#ifndef TSPIKETABLE_H
#define TSPIKETABLE_H

#include <vector>

#include "H5Cpp.h"
#include "hdf5_hl.h"
#include <netdata/tspike.h>
#include "datasetio.h"

using namespace H5;

const int CACHESIZE =100; 

class TSpikeTable : public DatasetIO
{
 public:
  TSpikeTable(int src, H5::Group gloc); 
  ~TSpikeTable(); 
  void append(const RawData *); 
  void flush();
  
 private:
  int src_; 
  std::vector<TSpike_t> dataCache_; 
  H5::Group tableLoc_; 
  std::vector<size_t> dstSizes_; 
  std::vector<size_t> dstOffsets_; 
  std::string tableName_; 
  //size_t fieldTypes[]; 
  
}; 

#endif // TSPIKETABLE_H
