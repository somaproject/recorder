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

    class WaveTable; 
    typedef boost::shared_ptr<WaveTable> pWaveTable_t; 

    class WaveTable : public DatasetIO
      {
	static const int CACHESIZE =100; 
	
      public:
	static pWaveTable_t create(H5::Group containingGroup, std::string name, 
				   datasource_t src); 
	
	static pWaveTable_t open(H5::Group containingGroup, std::string name); 

	~WaveTable(); 
	

	void append(const pDataPacket_t ); 

	void flush();
	void updateName(); 
	std::string getPath(); 
	void appendStats(const SinkStats & stats);
	std::vector<SinkStats> getStats(); 

	datasource_t getSource() { return src_;} 


      private:

	WaveTable(std::string name, H5::Group gloc); 

	static void setSourceAttribute(H5::DataSet ds, datasource_t src); 
	static datasource_t getSourceAttribute(H5::DataSet ds); 

	
	H5::DataSet selfDataSet_; 
	std::vector<SinkStats> sinkStatsCache_; 

	static const int NFIELDS = 7; 

	void setupDataTypes(); 

	int src_; 
	std::vector<Wave_t> dataCache_; 
	H5::Group tableLoc_; 
	std::vector<size_t> dstSizes_; 
	std::vector<size_t> dstOffsets_; 
	std::string tableName_; 
	
      }; 
  }
}


#endif // WAVETABLE_H
