#ifndef TSPIKETABLE_H
#define TSPIKETABLE_H

#include <vector>

#include <H5Cpp.h>
#include <hdf5_hl.h>
#include <somanetwork/tspike.h>
#include "datasetio.h"
#include "sinkstats.h"
#include "h5helper.h"

using namespace H5;

namespace soma {
  namespace recorder {
    
    class TSpikeTable; 

    typedef boost::shared_ptr<TSpikeTable> pTSpikeTable_t; 


    class TSpikeTable : public DatasetIO
      {
	static const int CACHESIZE =100; 

      public:
	static pTSpikeTable_t create(H5::Group containingGroup, std::string name, 
				     datasource_t src); 

	static pTSpikeTable_t open(H5::Group containingGroup, std::string name); 

	~TSpikeTable(); 
	void append(const pDataPacket_t); 
	void flush();
	std::string getPath(); 
	void updateName(); 

	void appendStats(const SinkStats & stats);
	std::vector<SinkStats> getStats(); 

	datasource_t getSource() { return src_;} 

      private:

	TSpikeTable(std::string name, H5::Group gloc); 

	static const int NFIELDS = 6; 

	int src_; 
	std::vector<TSpike_t> dataCache_; 
	H5::Group tableLoc_; 
	std::vector<size_t> dstSizes_; 
	std::vector<size_t> dstOffsets_; 
	std::string tableName_; 
	static void setSourceAttribute(H5::DataSet ds, datasource_t src); 
	static datasource_t getSourceAttribute(H5::DataSet ds); 

	
	H5::DataSet selfDataSet_; 
	std::vector<SinkStats> sinkStatsCache_; 

	//size_t fieldTypes[]; 
	void setupDataTypes(); 

      }; 

  }
}


#endif // TSPIKETABLE_H
