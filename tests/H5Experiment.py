#!/usr/bin/python
"""
This is the python file to go along with the WaveTable_test
unit test.

The first argument is the name of the test to run

"""

import tables
import sys
from nose.tools import * 
import os

def timestamp_test(rootpath):
    f = tables.openFile(os.path.join(rootpath, 'H5Experiment_reftime.h5'))
    g = f.root.experiment
    assert_equal( g._v_attrs.referencetime, 0x12345678);
    
if __name__ == "__main__":
    if sys.argv[1] == "timestamp":
        timestamp_test(sys.argv[2])

