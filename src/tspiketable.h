#ifndef TSPIKETABLE_H
#define TSPIKETABLE_H

#include <vector>

#include "H5Cpp.h"
#include "hdf5_hl.h"
#include <somanetwork/tspike.h>
#include "datasetio.h"

using namespace H5;

const int CACHESIZE =100; 

class TSpikeTable : public DatasetIO
{
 public:
  TSpikeTable(datasource_t src, std::string name, 
	      H5::Group gloc); 
  ~TSpikeTable(); 
  void append(const pDataPacket_t); 
  void flush();
  
 private:
  int src_; 
  std::vector<TSpike_t> dataCache_; 
  H5::Group tableLoc_; 
  std::vector<size_t> dstSizes_; 
  std::vector<size_t> dstOffsets_; 
  std::string tableName_; 
  //void addNote(std::string str); 
  //std::list<std::string> getNotes(); 
    
  //size_t fieldTypes[]; 
  
}; 

#endif // TSPIKETABLE_H
