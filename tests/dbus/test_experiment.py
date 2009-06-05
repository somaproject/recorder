from nose.tools import *
import subprocess
import dbusserver
import tempfile
import dbus
import dbus.bus
import time
import tables
import socket
import os
import recorder_test_config
import canonical_data

dbusDaemon = None


def setup():
    global dbusDaemon
    dbusDaemon = dbusserver.LocalDBUSDaemon(dbusserver.tcpconfig)
    dbusDaemon.run()

def shutdown():
    global dbusDaemon
    dbusDaemon.close()
    
class SomaSockets(object):
    def __init__(self, dir):

        self.tfdir = dir
        self.dir = dir
        
        self.dataretx = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
        self.dataretx.bind("%s/dataretx" % self.tfdir)
        
        self.eventretx = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
        self.eventretx.bind("%s/eventretx" % self.tfdir)
        
        self.eventtx = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
        self.eventtx.bind("%s/eventtx" % self.tfdir)
    
def start_experiment(dbusDaemon):
    """
    Run the experiment independently in a temporary directory

    Use the domain socket interface so we can test-send socket
    data

    """
    
    tfdir  = tempfile.mkdtemp()
    filename = tfdir + "/test.h5"

    # now we have to create the test sockets
    sock = SomaSockets(tfdir)
    
    args = [recorder_test_config.soma_experiment_binary,
            "--create-file=%s" % filename,
            "--domain-socket-dir=%s" % tfdir,
            "--request-dbus-name=%s" % "soma.recording.Experiment",
            "--dbus=%s" % dbusDaemon.address,
            "--no-register",
            "--log-level=0"]
    print "args = ", args
    proc = subprocess.Popen(args)
    time.sleep(1) # FIXME : Race condition
    return proc, filename, sock
    
def test_launch():
    """
    Simply launch the experiment runner and see that we show up
    properly on the bus
    """
    
    global dbusDaemon
    proc, filename, sockdir = start_experiment(dbusDaemon)

    # now check that the device lives on the bus
    dbus_test_bus = dbus.bus.BusConnection(dbusDaemon.address)
    time.sleep(1) # FIXME : Race condition

    proxy = dbus_test_bus.get_object("soma.recording.Experiment",
                                     "/soma/recording/experiment")
    proxyif = dbus.Interface(proxy, "soma.recording.Experiment")
    assert_equal(proxyif.GetName(), filename)
    #proxy.GetFileProperties()
    
    proc.terminate()

def test_file_create():
    """
    Create the file, create a few epochs, and then save and close
    and try reading the file with pytables
    """
    
    global dbusDaemon

    proc, filename, sockdir = start_experiment(dbusDaemon)
    
    # now check that the device lives on the bus
    dbus_test_bus = dbus.bus.BusConnection(dbusDaemon.address)

    p = dbus_test_bus.get_object("soma.recording.Experiment",
                                     "/soma/recording/experiment")
    proxyif = dbus.Interface(p, "soma.recording.Experiment")
    assert_equal(proxyif.GetName(), filename)

    # assert there are no epochs
    assert_equal(proxyif.GetEpochs(), [])

    # create an epoch
    epoch_name1 = "test1"
    obj = proxyif.CreateEpoch(epoch_name1)

    assert_equal(len(proxyif.GetEpochs()), 1)
    epoch_path = proxyif.GetEpoch(epoch_name1)
    assert_equal(obj, epoch_path)

    # create a second epoch
    epoch_name2 = "test2"
    epoch_obj2 = proxyif.CreateEpoch(epoch_name2)

    assert_equal(len(proxyif.GetEpochs()), 2)
    epoch_path = proxyif.GetEpoch(epoch_name2)
    assert_equal(epoch_obj2, epoch_path)

    # create a third epoch
    epoch_name3 = "test3"
    obj = proxyif.CreateEpoch(epoch_name3)

    assert_equal(len(proxyif.GetEpochs()), 3)
    epoch_path = proxyif.GetEpoch(epoch_name3)
    assert_equal(obj, epoch_path)

    # now rename the second epoch
    epoch2_new_name = "test7"
    proxyif.RenameEpoch(epoch_obj2, epoch2_new_name)
    
    
    proxyif.Close()
    
    proc.wait()
    # now read the hdf5 file
    h5f = tables.openFile(filename, 'r')
    assert_true(epoch_name1 in h5f.root._v_children)
    assert_true(epoch2_new_name in h5f.root._v_children)
    assert_true(epoch_name3 in h5f.root._v_children)


def test_epoch_create_signal():
    """
    Create the file, create an epoch, and see if we correctly
    fire the signal. 
    """
    
    global dbusDaemon

    proc, filename, sockdir = start_experiment(dbusDaemon)
    
    # now check that the device lives on the bus
    from dbus.mainloop.glib import DBusGMainLoop
    import gobject

    DBusGMainLoop(set_as_default=True)
    
    dbus_loop = gobject.MainLoop() # 

    dbus_test_bus = dbus.bus.BusConnection(dbusDaemon.address)
    
    
    p = dbus_test_bus.get_object("soma.recording.Experiment",
                                     "/soma/recording/experiment")
    proxyif = dbus.Interface(p, "soma.recording.Experiment")
    assert_equal(proxyif.GetName(), filename)


    signalcalled = []
    
    def signaltest(testarg):
        print 'SIGNAL TEST'
        signalcalled.append(testarg)
        return False

    p.connect_to_signal("EpochCreate", signaltest)

    def runtest():
        # create an epoch
        epoch_name1 = "test1"
        obj = proxyif.CreateEpoch(epoch_name1)
        return False

    def quittest():
        dbus_loop.quit()
        return False
        
    source_id = gobject.timeout_add(0, runtest)
    source_id = gobject.timeout_add(1000, quittest)
      
    dbus_loop.run()

    proxyif.Close()
    
    proc.wait()
    assert_equal(len(signalcalled), 1)
    assert_equal(signalcalled[0], "/soma/recording/experiment/0")

def test_timeline():
    """
    1. create a file
    2. send a time event, 0x0
    3. get reference time
    4. send a time event, 0x110
    5. mark the reference time
    6. Get reference time

    Check that :
    1. the new reference time matches
    the sent time event

    2. we correctly send the "new reference time" signal
        
    """
    
    global dbusDaemon


    proc, filename, socks = start_experiment(dbusDaemon)


    # Set up main loop
    from dbus.mainloop.glib import DBusGMainLoop
    import gobject

    DBusGMainLoop(set_as_default=True)
    
    dbus_loop = gobject.MainLoop() # 

    
    # now check that the device lives on the bus
    dbus_test_bus = dbus.bus.BusConnection(dbusDaemon.address)

    p = dbus_test_bus.get_object("soma.recording.Experiment",
                                     "/soma/recording/experiment")
    exp_proxyif = dbus.Interface(p, "soma.recording.Experiment")
    time_proxyif = dbus.Interface(p, "soma.recording.Timeline")

    signalcalled = []
    def referenceTimeCallback(testarg):
        print "Reference time change"
        signalcalled.append(testarg)
        return False

    p.connect_to_signal("ReferenceTimeChange", referenceTimeCallback)

    soma_eventtx = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)

    times = []
    def runtest():
        p0 = canonical_data.somaSendTimeEvent(0x100, 0)
        soma_eventtx.sendto(p0, os.path.join(socks.tfdir, "eventrx"))
        times.append( time_proxyif.GetReferenceTime())
        p1 = canonical_data.somaSendTimeEvent(0x110, 1)

        soma_eventtx.sendto(p1, os.path.join(socks.tfdir, "eventrx"))

        time_proxyif.MarkReferenceTime()
        times.append(time_proxyif.GetReferenceTime())
        
        return False

    def quittest():
        dbus_loop.quit()
        return False
        
    source_id = gobject.timeout_add(0, runtest)
    source_id = gobject.timeout_add(1000, quittest)
      
    dbus_loop.run()

    exp_proxyif.Close()
    
    proc.wait()
    assert_equal( times, [0x0, 0x110])
    assert_equal(signalcalled, [0x110])
    
    

def teardown():
    global dbusDaemon
    dbusDaemon.close()


