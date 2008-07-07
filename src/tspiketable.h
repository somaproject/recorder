#ifndef TSPIKETABLE_H
#define TSPIKETABLE_H

#include <vector>

#include <H5Cpp.h>
#include <hdf5_hl.h>
#include <somanetwork/tspike.h>
#include "datasetio.h"

using namespace H5;

namespace soma {
  namespace recorder {
    
    class TSpikeTable : public DatasetIO
      {
	static const int CACHESIZE =100; 
	
      public:
	TSpikeTable(datasource_t src, std::string name, 
		    H5::Group gloc); 
	TSpikeTable(std::string name, H5::Group gloc); 
	~TSpikeTable(); 
	void append(pDataPacket_t); 
	void flush();
	
      private:
	int src_; 
	std::vector<TSpike_t> dataCache_; 
	H5::Group tableLoc_; 
	std::vector<size_t> dstSizes_; 
	std::vector<size_t> dstOffsets_; 
	std::string tableName_; 
	void setSourceInFile(datasource_t src); 
	datasource_t getSourceFromFile(); 

	//void addNote(std::string str); 
	//std::list<std::string> getNotes(); 
	
	//size_t fieldTypes[]; 
	
      }; 
    
  }
}


#endif // TSPIKETABLE_H
