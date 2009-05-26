#!/usr/bin/python
"""
This is the python file to go along with the WaveTable_test
unit test.

The first argument is the name of the test to run

"""

import tables
import sys

def typecheck(t, name, type):
    assert isinstance(t.coldescrs[name], type)
        
    

def appenddata1_test():
    
    f = tables.openFile('H5Epoch_appenddata1.h5')
    g = f.root.myEpoch

    # check that we created the groups correctly
    hipp1 = g.TSpike.hipp1
    hipp4 = g.Wave.hipp4
    
    assert len(hipp1) == 100 
    assert len(hipp4) == 200
    f.close()
    
def notecheck_test():
    
    f = tables.openFile('H5Epoch_notetest_diskcheck.h5')
    
    g = f.root.testGroup
    table = g.TestEpoch.notes

    assert len(table) == 3
    assert table[2]['name'] == 'test2'
    
    
    f.close()

if __name__ == "__main__":
    if sys.argv[1] == "appenddata1":
        appenddata1_test()
    elif sys.argv[1] == "notecheck":
        notecheck_test()
        
