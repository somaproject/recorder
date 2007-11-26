#!/usr/bin/python
"""
This is the python file to go along with the H5FileRecorder_create
unit test.

The first argument is the name of the test to run

"""

import tables
import sys


def createepoch_test():
    t = tables.openFile('H5FileRecorder_createepoch.h5')
    table = t.root.TestEpoch
    table2 = t.root.TestEpoch2
        
def createTables_test():
    t = tables.openFile('H5FileRecorder_createTables.h5')
    epoch = t.root.TestEpoch
    N = 32
    assert len(epoch.tspikes._v_children) == N
    assert len(epoch.waves._v_children) == N
    for i in range(N):
        assert ("chan%02d" % i ) in epoch.tspikes._v_children
        assert ("chan%02d" % i ) in epoch.waves._v_children

    # we should also check the data type here
    

if __name__ == "__main__":
    if sys.argv[1] == "createepoch":
        createepoch_test()
    elif sys.argv[1] == "createTables":
        createTables_test()
        
        
