#ifndef H5HELPER_H
#define H5HELPER_H

#include <boost/algorithm/string.hpp>
#include <H5Cpp.h>
#include <string>
#include <vector>
#include "sinkstats.h"


namespace soma {
  namespace recorder {
    namespace h5helper { 
      
      inline std::string getPath(H5::Group obj) {
	char name[2000]; 
	hid_t id = obj.getId();
	H5Iget_name(id, name, 2000); 
	
	return std::string(name); 
	
      }
      
      inline std::string getPath(H5::DataSet obj) {
	char name[2000]; 
	hid_t id = obj.getId();
	H5Iget_name(id, name, 2000); 
	
	return std::string(name); 
	
      }
      
      inline std::string extractName(std::string path) {
	// returns the final bit of the name
	typedef std::vector< std::string > split_vector_type;
	split_vector_type SplitVec; // #2: Search for tokens
	boost::split( SplitVec, path, boost::is_any_of("/") );
	return SplitVec.back(); 
      }
      
      inline std::string extractBase(std::string path) {
	// returns the final bit of the name
	int pos = path.find_last_of("/"); 
	return path.substr(0, pos); 	
      }
      
      inline std::string getName(H5::Group obj) {
	std::string path = getPath(obj); 
	
	return extractName(path); 
      }
      
      inline void writeStatsAttribute(H5::DataSet obj, const std::vector<SinkStats> & stats)
      {
	/* 
	   Helper functon to write a vector of stats to a dataset node
	   
	*/ 
	H5::CompType sinkStatsType(sizeof(SinkStats)); 
	sinkStatsType.insertMember("startts", HOFFSET(SinkStats, pktCount), H5::PredType::NATIVE_UINT); 
	sinkStatsType.insertMember("latestSeq", HOFFSET(SinkStats, latestSeq), H5::PredType::NATIVE_UINT); 
	sinkStatsType.insertMember("dupeCount", HOFFSET(SinkStats, dupeCount), H5::PredType::NATIVE_UINT); 
	sinkStatsType.insertMember("pendingCount", HOFFSET(SinkStats, pendingCount), H5::PredType::NATIVE_UINT); 
	sinkStatsType.insertMember("missingPacketCount", HOFFSET(SinkStats, missingPacketCount), H5::PredType::NATIVE_UINT); 
	sinkStatsType.insertMember("reTxRxCount", HOFFSET(SinkStats, reTxRxCount), H5::PredType::NATIVE_UINT); 
	sinkStatsType.insertMember("outoOfOrderCount", HOFFSET(SinkStats, outOfOrderCount), H5::PredType::NATIVE_UINT); 
	
	const int LENGTH = stats.size(); 
	const int RANK = 1; 

	hsize_t dim[] = {LENGTH};   /* Dataspace dimensions */
	H5::DataSpace space( RANK, dim );

	H5::Attribute sinkattribute; 
	bool exists = false; 
	for (int i = 0; i < obj.getNumAttrs(); i++) {
	  H5::Attribute a = obj.openAttribute(i); 
	  if (a.getName() == "stats") {
	    exists = true; 
	    sinkattribute = a; 
	    break; 
	  } 

	}
	
	
	if (!exists) {
	  sinkattribute = 
	    obj.createAttribute("stats", sinkStatsType, space);
	}
  
	sinkattribute.write(sinkStatsType, &stats[0]); 
	

      }
      inline const std::vector<SinkStats>  loadStatsAttribute(H5::DataSet obj)
      {
	/* 
	   Helper functon to read a vector of stats from a dataset node
	*/ 
	H5::CompType sinkStatsType(sizeof(SinkStats)); 
	sinkStatsType.insertMember("startts", HOFFSET(SinkStats, pktCount), H5::PredType::NATIVE_UINT); 
	sinkStatsType.insertMember("latestSeq", HOFFSET(SinkStats, latestSeq), H5::PredType::NATIVE_UINT); 
	sinkStatsType.insertMember("dupeCount", HOFFSET(SinkStats, dupeCount), H5::PredType::NATIVE_UINT); 
	sinkStatsType.insertMember("pendingCount", HOFFSET(SinkStats, pendingCount), H5::PredType::NATIVE_UINT); 
	sinkStatsType.insertMember("missingPacketCount", HOFFSET(SinkStats, missingPacketCount), H5::PredType::NATIVE_UINT); 
	sinkStatsType.insertMember("reTxRxCount", HOFFSET(SinkStats, reTxRxCount), H5::PredType::NATIVE_UINT); 
	sinkStatsType.insertMember("outoOfOrderCount", HOFFSET(SinkStats, outOfOrderCount), H5::PredType::NATIVE_UINT); 
	// FIXME IMPLEMENT 

	
      }


    }
  }
}

#endif // H5HELPER_H
