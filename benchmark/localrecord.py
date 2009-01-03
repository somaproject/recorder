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
import pygtk
from cStringIO import StringIO
from padnums import pprint_table


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
                                       "--log-level=1"])
        
        self.name = name
        time.sleep(0.5)
        
    def close(self):
        # kill the thing
        os.kill(self.popen.pid, signal.SIGKILL)
        time.sleep(1)

def printstats(epoch):

    allstats = {}
    N = len(epoch.GetDataSinks())

    for i in range(N):
        stats = epoch.GetSinkSessionStatistics(i, 0, 0)
        for k, v in stats.iteritems():
            kstr = str(k)
            if kstr in allstats:
                allstats[kstr].append(int(v))
            else:
                allstats[kstr] = [int(v),]
    
    # convert
    header = ["src"]
    header += [str(x) for x in allstats.keys()]
    
    statslist = []
    for rowi in xrange(N):
        data = [str(rowi), ]

        for i in range(1, len(header)):

            key =  header[i]

            data.append(int(allstats[key][rowi]))
        statslist.append(data)

    table = [header] + statslist
    out = StringIO()
    pprint_table(out, table)
    print out.getvalue()

def test():
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

    expconn = iface.ListOpenExperiments()[0]
    print "The dbus connection is", expconn
    expobj = bus.get_object(expconn, "/soma/recording/experiment")
    exp = dbus.Interface(expobj, "soma.recording.Experiment")

    exp.GetFileProperties()

    # now we loop, each time selecting a new set of data
    epochRose = exp.CreateEpoch("Rose")
    epochRose = bus.get_object(expconn, epochRose)
    epochRose = dbus.Interface(epochRose, "soma.recording.Epoch")
    
    # ENABLE DATA IN EPOCHS
    N = 64
    for i in range(N):
        epochRose.EnableDataSink(i, 0)

    epochRose.StartRecording()
    
    for i in xrange(1000):
        for src in xrange(N):
            epochRose.GetSinkSessionStatistics(src,  0, 0)
            
    epochRose.StopRecording()        
    printstats(epochRose)
    
    print "Now we're going to close the experiment!!!"
    
    exp.Close()
    runner.close()
    
    
    
if __name__ == "__main__":
    test()
