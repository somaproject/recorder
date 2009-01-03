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
import tempfile
import random

RECPATH = "../src/soma-recorder"
EXPPATH = "../src/soma-experiment"
SOMAIP = "127.0.0.1"

def getTempFileName():
    alphabet = 'abcdefghijklmnopqrstuvwxyz'
    fname = ""
    for i in range(10):
        for x in random.sample(alphabet, 1):
            fname += x
    fname += ".h5"
    print "THE FILE NAME IS ", fname
    return fname


class RecorderRunner(object):
    def __init__(self, recpath, name):

        # set the name to be whatever
        self.popen = subprocess.Popen([recpath,
                                       "--soma-ip=%s" % SOMAIP, 
                                       "--experimentbin=%s" % EXPPATH,
                                       "--log-level=0"])
        
        self.name = name
        time.sleep(0.5)
        
    def close(self):
        # kill the thing
        os.kill(self.popen.pid, signal.SIGKILL)
        time.sleep(1)



    
    
class TestCreate(unittest.TestCase):

    def testCreateRecorder(self):
        runner = RecorderRunner(RECPATH, "soma.recording.Recorder")
        bus = dbus.SessionBus()


        proxy = bus.get_object(runner.name,
                               "/soma/recording/recorder")
        iface = dbus.Interface(proxy, "soma.recording.Recorder")
        iface.ListOpenExperiments()
        
        runner.close()

    def testCreateExperiment(self):
        runner = RecorderRunner(RECPATH, "soma.recording.Recorder")
        bus = dbus.SessionBus()


        proxy = bus.get_object(runner.name,
                               "/soma/recording/recorder")
        iface = dbus.Interface(proxy, "soma.recording.Recorder")
        iface.ListOpenExperiments()

        fname = getTempFileName()
        iface.CreateExperiment(fname)
        
        while (len(iface.ListOpenExperiments()) == 0):
            time.sleep(1)

        runner.close()
