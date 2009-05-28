from nose.tools import *
import subprocess
import dbusserver
import tempfile
import dbus
import dbus.bus
import time
import tables
import socket

dbusDaemon = None

soma_recorder_binary = "../../src/recordermanager.py"


def setup():
    global dbusDaemon
    dbusDaemon = dbusserver.LocalDBUSDaemon(dbusserver.tcpconfig)
    dbusDaemon.run()

def start_recorder(dbusDaemon, somaip, mock=True, expbin = None):
    """
    Run the recorder with mock objects independently in a temporary directory

    """
    
    tfdir  = tempfile.mkdtemp()
    
    args = [soma_recorder_binary,
            "--dbus=%s" % dbusDaemon.address,
            "--expdir=%s" % tfdir,
            "--soma-ip=%s" % somaip]
    if mock:
        args.append("--mock")
    else:
        args.append("--expbin=%s" % expbin)
        
    
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
    Try and create an actual experiment, by using a real binary? 
    """
    
    global dbusDaemon
    proc, tgtdir = start_recorder(dbusDaemon, "127.0.0.1", False,
                                  "../../src/soma-experiment")


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
    
    
    proc.terminate()
    
    
def teardown():
    global dbusDaemon
    dbusDaemon.close()


