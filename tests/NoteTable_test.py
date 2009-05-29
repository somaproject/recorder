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
    
    
def diskcheck_test():
    
    f = tables.openFile('NoteTable_diskcheck.h5')
    g = f.root.testGroup
    table = g.notes

    assert len(table) == 3
    assert table[2]['name'] == 'test2'
    
    
    
if __name__ == "__main__":
    if sys.argv[1] == "diskcheck":
        diskcheck_test()
    elif sys.argv[1] == "append":
        append_test()
        
        
