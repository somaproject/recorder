#!/usr/bin/python
from tables import * 
# Open a file in "w"rite mode
h5file = openFile('test.h5')

print h5file

#print h5file.root.test1.tspikes.chan0._v_children
