#!/usr/bin/python
from tables import * 

class Particle(IsDescription):
    name      = StringCol(16)   # 16-character String
    idnumber  = Int64Col()      # Signed 64-bit integer
    ADCcount  = UInt16Col()     # Unsigned short integer
    TDCcount  = UInt8Col()      # unsigned byte
    grid_i    = Int32Col()      # integer
    grid_j    = Int32Col()      # integer
    pressure  = Float32Col()    # float  (single-precision)
    energy    = Float64Col()    # double (double-precision)

print
print   '-**-**-**-**-**-**- file creation  -**-**-**-**-**-**-**-'

# The name of our HDF5 filename
filename = "tutorial1.h5"

print "Creating file:", filename

# Open a file in "w"rite mode
h5file = openFile(filename, mode = "w", title = "Test file")

print
print   '-**-**-**-**-**- group and table creation  -**-**-**-**-**-**-**-'

# Create a new group under "/" (root)
group = h5file.createGroup("/", 'detector', 'Detector information')
print "Group '/detector' created"

# Create one table on it
table = h5file.createTable(group, 'readout', Particle, "Readout example")
print "Table '/detector/readout' created"

# Print the file
print h5file
print
print repr(h5file)

