#!/usr/bin/python
"""
This is the python file to go along with the TSpikeTable_test
unit test.

The first argument is the name of the test to run

"""

import tables
import sys
from nose.tools import * 

def typecheck(t, name, type):
    assert isinstance(t.coldescrs[name], type)
        
    

def create_test():
    t = tables.openFile('TSpikeTable_create.h5')
    g = t.root.testGroup
    t = t.root.testGroup.hipp1

    assert isinstance(t.coldescrs['src'],
                       tables.description.UInt8Col)
    
    assert isinstance(t.coldescrs['time'],
                       tables.description.UInt64Col)

    for chan in ['x', 'y', 'a', 'b']:
        typecheck(t, "%s/threshold" % chan,
                  tables.description.Int32Col)
        
        typecheck(t, "%s/valid" % chan,
                  tables.description.UInt8Col)
        
        typecheck(t, "%s/wave" % chan,
                  tables.description.Int32Col)
        assert t.coldescrs["%s/wave" % chan].shape == (32,)

    # check the attribute is correctly written
    assert t.attrs.src == 17
    
def append_test():

    f = tables.openFile('TSpikeTable_append.h5')
    g = f.root.testGroup
    table = g.hipp2

    N = 1000
    data = table.read()
    # we need to make this part of the tests work with pytables
    for i, d in enumerate(data):
        assert d['src'] == 23 
        
        assert d['time'] == i
        chans = ['x', 'y', 'a', 'b']
        
        for j in range(len(chans)):
            assert_equal(d[chans[j]]['filtid'],  i * 5 + j)
            assert d[chans[j]]['valid'] == 1
            assert d[chans[j]]['threshold'] == i * 0x1234 + j*0xAB
            
            for k in range(32):
                assert d[chans[j]]['wave'][k] == i + j + k 
                
    
    
if __name__ == "__main__":
    if sys.argv[1] == "create":
        create_test()
    elif sys.argv[1] == "append":
        append_test()
        
        
