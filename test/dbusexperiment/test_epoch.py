
from nose.tools import *
import subprocess
import dbusserver
import tempfile
import dbus
import dbus.bus
import time
import tables
import socket
import os.path
import struct

import test_experiment
import canonical_data

dbusDaemon = None

def setup():
    global dbusDaemon
    dbusDaemon = dbusserver.LocalDBUSDaemon(dbusserver.tcpconfig)
    dbusDaemon.run()

def teardown():
    global dbusDaemon
    dbusDaemon.close()
    
def test_epoch_create():
    """
    Can we create a single epoch?

    """
    
    global dbusDaemon
    proc, filename, sockdir = test_experiment.start_experiment(dbusDaemon)
    
    # now check that the device lives on the bus
    dbus_test_bus = dbus.bus.BusConnection(dbusDaemon.address)

    p = dbus_test_bus.get_object("soma.recording.Experiment",
                                     "/soma/recording/experiment")
    proxyif = dbus.Interface(p, "soma.recording.Experiment")

    # create an epoch
    epoch_name1 = "test1"
    obj = proxyif.CreateEpoch(epoch_name1)
    proxyif.Close()
    
    proc.wait()
    # now read the hdf5 file
    h5f = tables.openFile(filename, 'r')
    assert_true(epoch_name1 in h5f.root._v_children)
    
    
def test_epoch_manipulate():
    """
    1. Create an epoch
    2. get access to the epoch proxy object
    3. enable some data
    4. Close the epoch
    """
    
    global dbusDaemon
    proc, filename, sockdir = test_experiment.start_experiment(dbusDaemon)
    
    # now check that the device lives on the bus
    dbus_test_bus = dbus.bus.BusConnection(dbusDaemon.address)

    p = dbus_test_bus.get_object("soma.recording.Experiment",
                                     "/soma/recording/experiment")
    experiment_if = dbus.Interface(p, "soma.recording.Experiment")

    # create an epoch
    epoch_name1 = "test1"

    epoch_p = experiment_if.CreateEpoch(epoch_name1)
    epoch_obj = dbus_test_bus.get_object("soma.recording.Experiment",
                                        epoch_p)
    epoch_if = dbus.Interface(epoch_obj, "soma.recording.Epoch")

    # now manipulate the epoch directly
    assert_equal(epoch_if.GetName(), epoch_name1)

    epoch_if.EnableDataSink(0, 0);
    epoch_if.SetDataName(0, "HelloData");
    epoch_if.StartRecording()
    epoch_if.StopRecording()
    experiment_if.Close()
    
    proc.wait()
    print "The H5 file name is", filename
    # now read the hdf5 file
    h5f = tables.openFile(filename, 'r')
    assert_true(epoch_name1 in h5f.root._v_children)

    # now things inside the epoch
    
    assert_true("HelloData" in h5f.root._v_children[epoch_name1].TSpike._v_children)
    

def test_epoch_save_data():
    """
    1. Create an epoch
    2. Enable some data sources
    3. Start recording.
    4. Write some data packets into the domain socket interface
    5. Verify those packets in the file.
    """
    global dbusDaemon
    proc, filename, sockdir = test_experiment.start_experiment(dbusDaemon)
    
    # now check that the device lives on the bus
    dbus_test_bus = dbus.bus.BusConnection(dbusDaemon.address)

    p = dbus_test_bus.get_object("soma.recording.Experiment",
                                     "/soma/recording/experiment")
    experiment_if = dbus.Interface(p, "soma.recording.Experiment")

    # create an epoch
    epoch_name1 = "test1"

    epoch_p = experiment_if.CreateEpoch(epoch_name1)
    epoch_obj = dbus_test_bus.get_object("soma.recording.Experiment",
                                        epoch_p)
    epoch_if = dbus.Interface(epoch_obj, "soma.recording.Epoch")

    # now manipulate the epoch directly
    assert_equal(epoch_if.GetName(), epoch_name1)

    epoch_if.EnableDataSink(0, 0);
    epoch_if.SetDataName(0, "HelloData");
    epoch_if.StartRecording()
    while(not epoch_if.GetRecordingState() ):
        pass # spin until we know we are "in recording" 
    
    # write some test data
    socktest = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
    data_send_sock_path = sockdir + "/" + "sending_data"
    socktest.bind(data_send_sock_path)

    
    assert_true(os.path.exists(sockdir + "/data/tspike/0"))
    for i in range(10):
        ts = canonical_data.createTSpike(0, i)
        seq = struct.pack(">i4", i)
        socktest.sendto(seq + ts.tostring(), sockdir + "/data/tspike/0")
    
    time.sleep(1) # wait for the data to get there
    epoch_if.StopRecording()
    experiment_if.Close()
    proc.wait()

    # now read the hdf5 file
    h5f = tables.openFile(filename, 'r')
    assert_true(epoch_name1 in h5f.root._v_children)

    # verify tspike data

    epochgrp = h5f.root._v_children[epoch_name1]
    assert_true("HelloData" in epochgrp.TSpike._v_children)
    assert_equal(len(epochgrp.TSpike.HelloData), 10)
    r = epochgrp.TSpike.HelloData.read()

    for i in range(10):
        canonical_data.verify_tspike(canonical_data.createTSpike(0, i),
                                     r[i])
        
        

def test_epoch_sessions():
    """
    Test if we get the session interface works correctly:
    1. can we start and stop with multiple sessions?
    2. when recording is occuring, do we correctly prevent
    changing of parameters
    3. do we get the correct total number of sessions?
    
    """
    
    global dbusDaemon
    proc, filename, sockdir = test_experiment.start_experiment(dbusDaemon)
    print "test epoch session filename = ", filename
    # now check that the device lives on the bus
    dbus_test_bus = dbus.bus.BusConnection(dbusDaemon.address)

    p = dbus_test_bus.get_object("soma.recording.Experiment",
                                     "/soma/recording/experiment")
    experiment_if = dbus.Interface(p, "soma.recording.Experiment")

    # create an epoch
    epoch_name1 = "test1"

    epoch_p = experiment_if.CreateEpoch(epoch_name1)
    epoch_obj = dbus_test_bus.get_object("soma.recording.Experiment",
                                        epoch_p)
    epoch_if = dbus.Interface(epoch_obj, "soma.recording.Epoch")

    # now manipulate the epoch directly
    assert_equal(epoch_if.GetName(), epoch_name1)

    epoch_if.EnableDataSink(0, 0);
    epoch_if.SetDataName(0, "HelloData");

    
    # write some test data
    socktest = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
    data_send_sock_path = sockdir + "/" + "sending_data"
    socktest.bind(data_send_sock_path)
    

    seq = 0
    sessioncnts = [(10, 100), (20, 50),
                   (50, 50), (100, 40)]
    for session_send, not_send in sessioncnts:
        epoch_if.StartRecording()
        while(not epoch_if.GetRecordingState() ):
            pass # spin until we know we are "in recording" 

        while not os.path.exists(sockdir + "/data/tspike/0"):
            pass
        
        for i in range(session_send):
            ts = canonical_data.createTSpike(0, i)
            seqstr = struct.pack(">i4", seq)
            socktest.sendto(seqstr + ts.tostring(), sockdir + "/data/tspike/0")
            seq += 1
        
        time.sleep(1) # wait for the data to get there

        epoch_if.StopRecording()
        while(epoch_if.GetRecordingState() ):
            pass # spin until we know we are done recording
        
    experiment_if.Close()
    proc.wait()

    # now read the hdf5 file
    h5f = tables.openFile(filename, 'r')
    assert_true(epoch_name1 in h5f.root._v_children)

    # verify tspike data

    epochgrp = h5f.root._v_children[epoch_name1]
    r = epochgrp.TSpike.HelloData.read()

    pos = 0
    for session_send, not_send in sessioncnts:
        for i in range(session_send):
            canonical_data.verify_tspike(canonical_data.createTSpike(0, i),
                                         r[pos])
            pos += 1
