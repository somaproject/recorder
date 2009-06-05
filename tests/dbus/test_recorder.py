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
dbusDaemon = None




def setup():
    global dbusDaemon
    dbusDaemon = dbusserver.LocalDBUSDaemon(dbusserver.tcpconfig)
    dbusDaemon.run()

def start_recorder(dbusDaemon, somaip, mock=True, expbin = None):
    """
    Run the recorder with mock objects independently in a temporary directory

    """
    
    tfdir  = tempfile.mkdtemp()
    
    args = [recorder_test_config.soma_recorder_binary,
            "--dbus=%s" % dbusDaemon.address,
            "--expdir=%s" % tfdir,
            "--soma-ip=%s" % somaip]
    if mock:
        args.append("--mock")
    else:
        args.append("--expbin=%s" % expbin)
        
    print args
    proc = subprocess.Popen(args)
    time.sleep(1) # FIXME : Race condition
    return proc, tfdir
    
def test_launch():
    """
    Simply launch the experiment runner and see that we show up
    properly on the bus
    """
    
    global dbusDaemon
    proc, tgtdir = start_recorder(dbusDaemon, "127.0.0.1")

    # now check that the device lives on the bus
    dbus_test_bus = dbus.bus.BusConnection(dbusDaemon.address)
    time.sleep(1) # FIXME : Race condition

    proxy = dbus_test_bus.get_object("soma.recording.Manager",
                                     "/manager")
    proxyif = dbus.Interface(proxy, "soma.recording.Manager")
    assert_equal(proxyif.ListOpenExperiments(), [])
    
    proc.terminate()
    
def test_mock_experiment_creation():
    """
    Launch experiment interface, create mock expeirments, close them
    
    """
    
    global dbusDaemon
    proc, tgtdir = start_recorder(dbusDaemon, "127.0.0.1")
    fid = file("%s/test.h5" % tgtdir, 'w')
    fid.write("Test")
    fid.close()
    

    # now check that the device lives on the bus
    dbus_test_bus = dbus.bus.BusConnection(dbusDaemon.address)
    time.sleep(1) # FIXME : Race condition

    proxy = dbus_test_bus.get_object("soma.recording.Manager",
                                     "/manager")
    proxyif = dbus.Interface(proxy, "soma.recording.Manager")

    assert_equal(proxyif.ListAvailableExperiments(), ['test.h5'])

    # now we create an experiment
    proxyif.CreateExperiment("SillyExp.h5")

    while len(proxyif.ListOpenExperiments() ) == 0:
        pass
    
    
    proc.terminate()
    
def test_actual_experiment_creation():
    """
    Try and create an actual experiment, by using the actual
    "soma-experiment" binary. We only make sure that we
    successfully run the experiment here, and verify at the
    end that the file has been created and can be read. 
    
    """
    
    global dbusDaemon
    proc, tgtdir = start_recorder(dbusDaemon, "127.0.0.1", False,
                                  recorder_test_config.soma_experiment_binary)


    # now check that the device lives on the bus
    dbus_test_bus = dbus.bus.BusConnection(dbusDaemon.address)
    time.sleep(1) # FIXME : Race condition

    proxy = dbus_test_bus.get_object("soma.recording.Manager",
                                     "/manager")
    proxyif = dbus.Interface(proxy, "soma.recording.Manager")

    # now we create an experiment
    proxyif.CreateExperiment("SillyExp.h5")

    while len(proxyif.ListOpenExperiments() ) == 0:
        pass

    # connect to this experiment and close it
    exp_conn = proxyif.ListOpenExperiments()[0]
    exp_proxy = dbus_test_bus.get_object(exp_conn,
                                     "/soma/recording/experiment")
    exp_proxyif = dbus.Interface(exp_proxy, "soma.recording.Experiment")
    exp_proxyif.CreateEpoch("testepoch")
    exp_proxyif.Close()

    while len(proxyif.ListOpenExperiments() ) >  0:
        time.sleep(1)

    time.sleep(1)
    tgtf = os.path.join(tgtdir, "SillyExp.h5")
    assert_true(os.path.exists(tgtf))
    # try and open the file, which should be a valid HDF5 file
    table = tables.openFile(tgtf, 'r')
    assert_true("testepoch" in table.root._v_children)
    table.close()
    
    
    proc.terminate()
    
    
def teardown():
    global dbusDaemon
    dbusDaemon.close()


