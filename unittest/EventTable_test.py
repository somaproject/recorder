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
    t = tables.openFile('EventTable_create.h5')
    g = t.root.testGroup
    setable = t.root.testGroup.Events

    
    typecheck(setable, "seq", tables.description.UInt32Col);
    typecheck(setable, "src", tables.description.UInt8Col);
    typecheck(setable, "cmd", tables.description.UInt8Col);

    assert setable.coldescrs["data"].shape == (5,)
    
    
def append_test():
    
    f = tables.openFile('EventTable_append.h5')
    g = f.root.testGroup
    setable = g.Events

    N = 1000
    emod = 17
    pos = 0
    for i in xrange(N):
        for ne in xrange(i % 17):
            
            assert setable[pos]['seq'] == i
            assert setable[pos]['cmd'] == ne
            assert setable[pos]['src'] == (i + 100) % emod
            for d in xrange(5):
                assert setable[pos]['data'][d] == d + i
            pos += 1
                
    
    
if __name__ == "__main__":
    if sys.argv[1] == "create":
        create_test()
    elif sys.argv[1] == "append":
        append_test()
        
        
