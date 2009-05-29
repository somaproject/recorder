import tables
h5file = tables.openFile('append.h5')

t = h5file.root.testGroup.chan23
x  = t.read()
print x[1][0]
    
