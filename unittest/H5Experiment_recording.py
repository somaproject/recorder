#!/usr/bin/python
"""
This is the python file to go along with the TSpikeTable_test
unit test.

The first argument is the name of the test to run

"""

import tables
import sys

def typecheck(t, name, type):
    assert isinstance(t.coldescrs[name], type)
        
    

def simplesave_test():
    f = tables.openFile('H5ExperimentRecording_simplesave.h5')
    g = f.root.Rose
    t = f.root.Rose.TSpike.Sink0
    assert len(t.read()) == 10;
    pos = 1000
    for row in t.read():
        assert row['time'] == pos
        pos += 1
    f.close()
    
        
def saveswitch_test():
    t = tables.openFile('H5ExperimentRecording_saveswitch.h5')
    tR = t.root.Rose.TSpike.Sink0
    tD = t.root.Donna.TSpike.Sink0
    assert len(tR.read()) == 10;
    assert len(tD.read()) == 10;
    pos = 1000
    for row in tR.read():
        assert row['time'] == pos
        pos += 1

    for row in tD.read():
        assert row['time'] == pos
        pos += 1
    t.close()
    
def datasets_test():
    t = tables.openFile('H5ExperimentRecording_datasets.h5')
    rdata = [0, 1, 2]
    ddata = [2, 3, 4]

    for r in rdata:
        
        tR = t.root.Rose.TSpike._v_children["Sink%d" % r]
        assert len(tR.read()) == 10;
        pos = 1000
        for row in tR.read():
            assert row['time'] == pos
            pos += 1
        
    for d in ddata:
        
        tD = t.root.Donna.TSpike._v_children["Sink%d" % d]
        assert len(tD.read()) == 10;
        pos = 1010
        for row in tD.read():
            assert row['time'] == pos
            pos += 1
    t.close()

def epochrename_test():
    t = tables.openFile('H5ExperimentRecording_epochrename.h5')
    rdata = [0, 1, 2]
    ddata = [0]

    for r in rdata:
        
        tR = t.root.Rose.TSpike._v_children["Sink%d" % r]
        assert len(tR.read()) == 10;
        pos = 1000
        for row in tR.read():
            assert row['time'] == pos
            pos += 1
        
    for d in ddata:
        
        tD = t.root.DoctorDonna.TSpike._v_children["Sink%d" % d]
        assert len(tD.read()) == 10;
        pos = 1010
        for row in tD.read():
            assert row['time'] == pos
            pos += 1
    t.close()


def session_test():
    f = tables.openFile('H5ExperimentRecording_sessiontest.h5')
    g = f.root.Rose
    t = f.root.Rose.TSpike.Sink0
    # FIXME NEED REAL TEST HERE
    f.close()
    
def stats_test():
    f = tables.openFile('H5ExperimentRecording_stats.h5')
    g = f.root.Rose
    t = f.root.Rose.TSpike.Sink0
    # FIXME NEED REAL TEST HERE
    print  t._v_attrs

    f.close()
    
if __name__ == "__main__":
    if sys.argv[1] == "simplesave":
        simplesave_test()
    elif sys.argv[1] == "saveswitch":
        saveswitch_test()
    elif sys.argv[1] == "sessiontest":
        session_test()
    elif sys.argv[1] == "statstest":
        stats_test()
    elif sys.argv[1] == "datasets":
        datasets_test()
    elif sys.argv[1] == "epochrename":
        epochrename_test()
        
        
