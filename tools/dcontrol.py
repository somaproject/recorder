import sys
import dbus
from timeit import Timer
import time
import re

bus = dbus.SessionBus()
ob = bus.get_object('org.soma.Recorder','/org/soma/Recorder')
recorder = dbus.Interface(ob, dbus_interface='org.soma.Recorder')

class DataReceiverStatus(object):
    def processResultString(self, s):
        sl = s.split(", ")
        dhash = {}
        for v in sl:
            x = v.split("=")        
            dhash[x[0]] = int(x[1])

        return dhash

    def __init__(self, src, mstr):
        xre = re.compile("src=(\d+) typ=(\d+)")
        [srcstr, typstr] = xre.match(src).groups()
        
        rhash = self.processResultString(mstr)
        
        self.type = int(typstr)
        self.src = int(srcstr)
        self.pktCount = rhash['pktCount']
        self.latestSeq =  rhash['latestSeq'] 
        self.dupeCount =  rhash['dupeCount'] 
        self.pendingCount =  rhash['pendingCount'] 
        self.missingPacketCount =  rhash['missingPacketCount'] 
        self.reTxRxCount =  rhash['reTxRxCount']



def networkstatus():
    return recorder.NetworkStats()

def setup():
    recorder.CreateFile("test.h5");
    recorder.CreateEpoch("testEpoch")
    recorder.SwitchEpoch("testEpoch")
    for i in range(64):
        recorder.EnableDataRX(i, 0); 
        
    recorder.StartRecording()
def done():
    recorder.StopRecording()
    recorder.CloseFile()
    
if sys.argv[1] == "start" :
    setup()
elif sys.argv[1] == "stop":
    
    done()
    
elif sys.argv[1] == "statusbench":
    
    t = Timer("networkstatus()", "from __main__ import networkstatus")
    N = 1000
    print t.timeit(number = N) /N
    
elif sys.argv[1] == "status":
    result = []
    for i in range(1):
        result.append(recorder.NetworkStats())
        time.sleep(1)

    for r in result:
        s = r["src=0 typ=0"]
        drs = DataReceiverStatus("src=0 typ=0", s)
        print drs.type, drs.src
        
