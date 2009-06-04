#!/usr/bin/python
"""
This is the python file to go along with the TSpikeTable_test
unit test.

The first argument is the name of the test to run

"""

import tables
import sys
import os

def typecheck(t, name, type):
    assert isinstance(t.coldescrs[name], type)
        
    

def rename_test(rootpath):
    t = tables.openFile(os.path.join(rootpath, 'H5Epoch_rename.h5'))
    g = t.root.Silly
    t1 = t.root.Silly.TSpike.HelloWorld0
    t2 = t.root.Silly.TSpike.GoodbyeWorld7

##     assert isinstance(t.coldescrs['src'],
##                        tables.description.UInt8Col)
    
##     assert isinstance(t.coldescrs['time'],
##                        tables.description.UInt64Col)

##     for chan in ['x', 'y', 'a', 'b']:
##         typecheck(t, "%s/threshold" % chan,
##                   tables.description.Int32Col)
        
##         typecheck(t, "%s/valid" % chan,
##                   tables.description.UInt8Col)
        
##         typecheck(t, "%s/wave" % chan,
##                   tables.description.Int32Col)
##         assert t.coldescrs["%s/wave" % chan].shape == (32,)

##     # check the attribute is correctly written
##     assert t.attrs.src == 17
    t.close()
if __name__ == "__main__":
    if sys.argv[1] == "rename":
        rename_test(sys.argv[2])
        
