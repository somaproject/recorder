#/usr/bin/python
"""
attempt to analyze the output of the benchmark

"""

#import numpy as n
import sys

fid = file(sys.argv[1])

res = []

# remove header:
fid.readline()

for l in fid.readlines():
    res.append([int(x) for x in l.split()])

#data = n.array(res)
data=res

rowsize = 288e6/500e3
print "The rowsize is ", rowsize, "bytes per row"

for d in data:
    print "rows = ", d[0], "csize = ", d[1], " bsize=", d[2], "write at ", " %3.2f MB/sec" % (rowsize*d[0]/d[3])
    
