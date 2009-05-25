
import numpy as np
from nose.tools import * 


WAVELEN = 32
tspikewave_desc = np.dtype([('valid', np.uint8),
                           ('null1', np.uint8),
                           ('null2', np.uint8),
                           ('null3', np.uint8),                           
                           ('filtid', '>i4'),
                           ('threshold', '>i4'),
                           ('wave', '%d>i4' % WAVELEN)])
tspike_desc = np.dtype([('typ', np.uint8),
                       ('src', np.uint8),
                       ('chanlen', np.uint16), 
                       ('time', '>u8'),
                       ('version', np.uint16),
                       ('x', tspikewave_desc),
                       ('y', tspikewave_desc),
                       ('a', tspikewave_desc),
                       ('b', tspikewave_desc) ])

def createTSpike(src, index):
    N = 1
    x = np.zeros(N, dtype=tspike_desc)
    r = x.tostring()
    # make sure this is correct
    offset = 14
    assert ( (1 + 3 + 4 + 4 + WAVELEN*4 ) * 4 + offset == float(len(r))/N)

    i = 0
    x[i]['typ'] = 0
    x[i]['src'] = src
    x[i]['time'] = index * 10215 + 0x12345678
    for j, s in enumerate(['x', 'y', 'a', 'b']):
        x[i][s]['filtid'] = (j * index)
        x[i][s]['valid'] = (j * index  % 256)
        x[i][s]['threshold'] = (j * index * (2*17-141))
        for k in range(WAVELEN):
            x[i][s]['wave'][k] = (j * index  * 0x12345 +  (k)) % 2**31

    return x
    
def verify_tspike(t1, t2):
    assert_equal(t1['src'], t2['src'])
    assert_equal(t1['time'], t2['time'])

    assert_equal(t1['x']['filtid'], t2['x']['filtid'])
                 
##     for j, s in enumerate(['x', 'y', 'a', 'b']):
##         for f in ['filtid', 'valid', 'threshold']:
##             assert_equal(t1[s][f], t2[s][f])
##         for k in range(WAVELEN):
##             assert_equal(t1[s]['wave'][k], t2[s]['wave'][k])

