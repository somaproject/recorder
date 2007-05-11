#ifndef TSPIKETABLE_H
#define TSPIKETABLE_H

#include "H5Cpp.h"
#include "hdf5_hl.h"
using namespace H5;

void createTSpikeTable(int src, hid_t loc); 

#endif // TSPIKETABLE_H
