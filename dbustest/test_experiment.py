#!/usr/bin/python
"""

Test the experiment main program.

"""

import unittest
import os
import os.path
import subprocess
import signal
import tempfile
import dbus
import time


EXPPATH = "../src/soma-experiment"
SOMAIP = "127.0.0.1"

class ExperimentRunner(object):
    def __init__(self, exppath, name):

        # temp file for the hdf5 stuff
        self.file = tempfile.NamedTemporaryFile()
        self.file.close()
        
        objname = name + "." + os.path.basename(self.file.name)
        # set the name to be whatever
        self.popen = subprocess.Popen([exppath, "--create-file=%s" % self.file.name, 
                                       "--soma-ip=%s" % SOMAIP, 
                                       "--request-dbus-name=%s" % objname,
                                       "--no-register",
                                       "--no-daemon"])
        
        self.name = objname
        self.filename = self.file.name
        time.sleep(0.5)
        
    def close(self):
        # kill the thing
        os.kill(self.popen.pid, signal.SIGKILL)
        time.sleep(1)


class TestCreate(unittest.TestCase):

    def testCreateFile(self):
        exprunner = ExperimentRunner(EXPPATH, "test.Object")
        bus = dbus.SessionBus()
        proxy = bus.get_object(exprunner.name,
                               "/soma/recording/experiment")
        iface = dbus.Interface(proxy, "soma.recording.Experiment")
        
        iface.GetFileProperties()
        self.assertEqual(exprunner.filename, iface.GetName())
        iface.Close()

        exprunner.close()


    def testCreateEpoch(self):
        objname = "test.Object"
        exprunner = ExperimentRunner(EXPPATH, objname)
        bus = dbus.SessionBus()

        proxy = bus.get_object(exprunner.name,
                               "/soma/recording/experiment")
        iface = dbus.Interface(proxy, "soma.recording.Experiment")
        epaths = set()
        for i in range(3):
            e = iface.CreateEpoch("TestEpoch%d" % i)
            eobj = bus.get_object(exprunner.name,
                                   e)
            epaths.add(e)
            eobj = dbus.Interface(eobj, "soma.recording.Epoch")

        
        s = set( iface.GetEpochs())
        self.assertEqual(epaths, s)
        iface.Close()

        exprunner.close()

        

    def testDataSinkRecording(self):
        objname = "test.Object"
        exprunner = ExperimentRunner(EXPPATH, objname)
        bus = dbus.SessionBus()

        proxy = bus.get_object(exprunner.name,
                               "/soma/recording/experiment")
        iface = dbus.Interface(proxy, "soma.recording.Experiment")
        experiment = iface
        e = experiment.CreateEpoch("TestEpoch")
        eobj = bus.get_object(exprunner.name,
                              e)
        eobj = dbus.Interface(eobj, "soma.recording.Epoch")

        enabletgt = set([0, 3, 6, 9, 12])
        for i in range(max(enabletgt) + 1):
            eobj.EnableDataSink(i, 0)
            eobj.SetDataName(i, "TestName%d" % i)
        for i in range(max(enabletgt) + 1):
            if i not in enabletgt:
                eobj.DisableDataSink(i, 0)
                
        ds = eobj.GetDataSinks()
        dstuple = [eval(s) for s in ds]
        self.assertEqual(set(dstuple), set([(x, 0) for x in enabletgt]))

        eobj.StartRecording()
        eobj.StopRecording()
        
        eobj.StartRecording()
        eobj.StopRecording()
        
        eobj.StartRecording()
        eobj.StopRecording()
        
        
        # check the sessions
        x = eobj.GetSessions()
        self.assertEqual(3, len(x))
        experiment.Close()

        # done
        exprunner.close()

    def testDataSinkRecordingSwitch(self):
        """
        can we start recording in one epoch and
        switch to recording in another
        """
        objname = "test.Object"
        exprunner = ExperimentRunner(EXPPATH, objname)
        bus = dbus.SessionBus()

        proxy = bus.get_object(exprunner.name,
                               "/soma/recording/experiment")
        iface = dbus.Interface(proxy, "soma.recording.Experiment")
        experiment = iface
        e1 = experiment.CreateEpoch("TestEpoch1")
        eobj1 = bus.get_object(exprunner.name,
                              e1)
        eobj1 = dbus.Interface(eobj1, "soma.recording.Epoch")

        e2 = experiment.CreateEpoch("TestEpoch2")
        eobj2 = bus.get_object(exprunner.name,
                              e2)
        eobj2 = dbus.Interface(eobj2, "soma.recording.Epoch")

        eobj1.EnableDataSink(0, 0)
        eobj1.EnableDataSink(2, 0)

        eobj1.StartRecording()
        eobj2.EnableDataSink(1, 0)
        eobj2.StartRecording()

        eobj2.StopRecording()

        experiment.Close()
        
        # done
        exprunner.close()

class TestEpochsErrors(unittest.TestCase):
    """
    Test the errors that we worry about when using
    the dbus interface
    """
    
    def testCreateEpochTwice(self):
        """
        can we start recording in one epoch and
        switch to recording in another
        """
        objname = "test.Object"
        exprunner = ExperimentRunner(EXPPATH, objname)
        bus = dbus.SessionBus()

        proxy = bus.get_object(exprunner.name,
                               "/soma/recording/experiment")
        iface = dbus.Interface(proxy, "soma.recording.Experiment")
        experiment = iface
        e1 = experiment.CreateEpoch("TestEpoch1")
        eobj1 = bus.get_object(exprunner.name,
                              e1)
        eobj1 = dbus.Interface(eobj1, "soma.recording.Epoch")

        self.assertRaises(dbus.DBusException, experiment.CreateEpoch, "TestEpoch1")

        experiment.Close()
        exprunner.close()


    def testEditEpochMidRecord(self):
        """
        If we record into an epoch does modification of the
        epoch trigger an error?
                
        """
        objname = "test.Object"
        exprunner = ExperimentRunner(EXPPATH, objname)
        bus = dbus.SessionBus()

        proxy = bus.get_object(exprunner.name,
                               "/soma/recording/experiment")
        iface = dbus.Interface(proxy, "soma.recording.Experiment")
        experiment = iface
        e1 = experiment.CreateEpoch("TestEpoch1")
        eobj1 = bus.get_object(exprunner.name,
                              e1)
        eobj1 = dbus.Interface(eobj1, "soma.recording.Epoch")

        eobj1.StartRecording()

        self.assertRaises(dbus.DBusException, eobj1.EnableDataSink, 10, 0)
        self.assertRaises(dbus.DBusException, eobj1.DisableDataSink, 10, 0)
        self.assertRaises(dbus.DBusException, eobj1.SetDataName, 10, "helloTest")

        
        eobj1.StopRecording()

        # once we've recorded, we can't edit
        self.assertRaises(dbus.DBusException, eobj1.EnableDataSink, 10, 0)
        self.assertRaises(dbus.DBusException, eobj1.DisableDataSink, 10, 0)
        self.assertRaises(dbus.DBusException, eobj1.SetDataName, 10, "helloTest")
        
        
        experiment.Close()
        
        exprunner.close()
