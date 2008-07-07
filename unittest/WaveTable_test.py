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
        
    

def create_test():
    t = tables.openFile('WaveTable_create.h5')
    g = t.root.testGroup
    t = t.root.testGroup.eeg1

    
    assert isinstance(t.coldescrs['src'],
                       tables.description.UInt8Col)
    
    assert isinstance(t.coldescrs['time'],
                       tables.description.UInt64Col)

    typecheck(t, "samprate", tables.description.UInt16Col);
    typecheck(t, "selchan", tables.description.UInt16Col);
    typecheck(t, "filterid", tables.description.UInt16Col);
    assert t.coldescrs["wave"].shape == (128,)
    
    assert t.attrs.src == 17
    
def append_test():
    
    f = tables.openFile('WaveTable_append.h5')
    g = f.root.testGroup
    table = g.eeg2

    N = 1000

    for i in xrange(N):
        assert table[i]['src'] == 23 
        
        assert table[i]['time'] == i * 0x0123456789AB
        assert table[i]['samprate'] == 0x789A
        assert table[i]['selchan'] == 0x1234
        assert table[i]['filterid'] == 0xAABB
        for j in range(128):
            assert table[i]['wave'][j] == i * 0xABCD + j*0x17
        

                
    
    
if __name__ == "__main__":
    if sys.argv[1] == "create":
        create_test()
    elif sys.argv[1] == "append":
        append_test()
        
        
