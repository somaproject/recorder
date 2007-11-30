#ifndef WAVETABLE_H
#define WAVETABLE_H

#include <vector>

#include "H5Cpp.h"
#include "hdf5_hl.h"
#include <somanetwork/wave.h>
#include "datasetio.h"

using namespace H5;

namespace soma {
  namespace recorder {

    class WaveTable : public DatasetIO
      {
	static const int CACHESIZE =100; 
	
      public:
	WaveTable(datasource_t src, std::string name, H5::Group gloc); 
	~WaveTable(); 
	void append(pDataPacket_t ); 
	void flush();
	
      private:
	int src_; 
	std::vector<Wave_t> dataCache_; 
	H5::Group tableLoc_; 
	std::vector<size_t> dstSizes_; 
	std::vector<size_t> dstOffsets_; 
	std::string tableName_; 
	
	//size_t fieldTypes[]; 
	
      }; 
  }
}


#endif // WAVETABLE_H
