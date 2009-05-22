from nose.tools import *
import subprocess
import dbusserver
import tempfile
import dbus
import dbus.bus
import time
import tables

dbusDaemon = None

soma_experiment_binary = "../../src/soma-experiment"
soma_ip = "127.0.0.1"

def setup():
    global dbusDaemon
    dbusDaemon = dbusserver.LocalDBUSDaemon(dbusserver.tcpconfig)
    dbusDaemon.run()

def start_experiment():
    global dbusDaemon
    
    tfdir  = tempfile.mkdtemp()
    filename = tfdir + "/test.h5"
    args = [soma_experiment_binary,
                             "--create-file=%s" % filename,
                             "--soma-ip=%s" % soma_ip,
                             "--request-dbus-name=%s" % "soma.recording.Experiment",
                             "--dbus=%s" % dbusDaemon.address,
                             "--no-register",
                             "--no-daemon"]
    print args
    
    proc = subprocess.Popen(args)
    time.sleep(1) # FIXME : Race condition
    return proc, filename
    
def test_launch():
    """
    Simply launch the experiment runner and see that we show up
    properly on the bus
    """
    
    global dbusDaemon
    
    tfdir  = tempfile.mkdtemp()
    filename = tfdir + "/test.h5"
    args = [soma_experiment_binary,
                             "--create-file=%s" % filename,
                             "--soma-ip=%s" % soma_ip,
                             "--request-dbus-name=%s" % "soma.recording.Experiment",
                             "--dbus=%s" % dbusDaemon.address,
                             "--no-register",
                             "--no-daemon"]
    
    proc = subprocess.Popen(args)

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

    proc, filename = start_experiment()
    
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
    
    

def teardown():
    global dbusDaemon
    dbusDaemon.close()


