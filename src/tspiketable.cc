#include <boost/format.hpp> 
#include <iostream>
#include <somanetwork/tspike.h>
#include <somanetwork/datapacket.h>

#include "tspiketable.h"
#include "h5helper.h"

using namespace soma::recorder; 


pTSpikeTable_t TSpikeTable::create(H5::Group containingGroup, std::string name, 
				   datasource_t src)
{

  pTSpikeTable_t ts(new TSpikeTable(name, containingGroup)); 

  hsize_t wave_dims[1] = {TSPIKEWAVE_LEN}; 
  hid_t wt = H5Tarray_create(H5T_NATIVE_INT32, 1, wave_dims);  

  hid_t TSpikeWave_type = H5Tcreate(H5T_COMPOUND, sizeof(TSpikeWave_t)); 
  H5Tinsert(TSpikeWave_type, "filtid", 
	    HOFFSET(TSpikeWave_t, filtid), H5T_NATIVE_UINT32); 
  H5Tinsert(TSpikeWave_type, "valid", 
	    HOFFSET(TSpikeWave_t, valid), H5T_NATIVE_UINT8); 
  H5Tinsert(TSpikeWave_type, "threshold", 
	    HOFFSET(TSpikeWave_t, threshold), H5T_NATIVE_INT32); 
  H5Tinsert(TSpikeWave_type, "wave", 
	    HOFFSET(TSpikeWave_t, wave), wt); 
  
  const char *TSpike_field_names[NFIELDS]  =
    { "src", "time", "x", "y", "a", "b"}; 
  
  hid_t TSpike_field_type[NFIELDS]; 
  TSpike_field_type[0] = H5T_NATIVE_UINT8; 
  TSpike_field_type[1] = H5T_NATIVE_UINT64; 
  TSpike_field_type[2] = TSpikeWave_type; 
  TSpike_field_type[3] = TSpikeWave_type; 
  TSpike_field_type[4] = TSpikeWave_type; 
  TSpike_field_type[5] = TSpikeWave_type; 

  herr_t     status;

  size_t TSpike_dst_size =  sizeof( TSpike_t );

  status = H5TBmake_table( "Table Title", 
			   ts->tableLoc_.getLocId(),
			   ts->tableName_.c_str(), 
			   NFIELDS, 
			   0, 
			   TSpike_dst_size, 
			   TSpike_field_names, 
			   &(ts->dstOffsets_[0]), 
			   TSpike_field_type,
			   100, NULL, 
			   false, NULL  );
  
  
  // get hid_t 
  ts->selfDataSet_ = ts->tableLoc_.openDataSet(ts->tableName_); 
  ts->src_ = src; 
  setSourceAttribute(ts->selfDataSet_, src); 
  
  return ts; 
  
}

pTSpikeTable_t TSpikeTable::open(H5::Group containingGroup, std::string name)
{
  
  pTSpikeTable_t ts(new TSpikeTable(name, containingGroup)); 
  
  // get object
  
  // configure dataset object
  ts->selfDataSet_ = ts->tableLoc_.openDataSet(ts->tableName_); 
  ts->src_ = getSourceAttribute(ts->selfDataSet_); 

  return ts; 
  
}


TSpikeTable::TSpikeTable(std::string name, 
			 H5::Group containingGroup) :
  dataCache_(),
  tableLoc_(containingGroup), 
  tableName_(name)
{
  // setup cache
  dataCache_.reserve(CACHESIZE); 
  setupDataTypes(); 
}


void TSpikeTable::setupDataTypes()
{

  // construct the table

  dstOffsets_.push_back(HOFFSET(TSpike_t, src)); 
  dstOffsets_.push_back(HOFFSET(TSpike_t, time)); 
  dstOffsets_.push_back(HOFFSET(TSpike_t, x)); 
  dstOffsets_.push_back(HOFFSET(TSpike_t, y)); 
  dstOffsets_.push_back(HOFFSET(TSpike_t, a)); 
  dstOffsets_.push_back(HOFFSET(TSpike_t, b)); 


  TSpike_t tests; 
  dstSizes_.push_back(sizeof( tests.src)); 
  dstSizes_.push_back(sizeof( tests.time)); 
  dstSizes_.push_back(sizeof( tests.x)); 
  dstSizes_.push_back(sizeof( tests.y)); 
  dstSizes_.push_back(sizeof( tests.a)); 
  dstSizes_.push_back(sizeof( tests.b)); 


}
void TSpikeTable::append(const pDataPacket_t rdp)
{
  // For performance reasons we append the incoming data packets to
  // an internal cache

  TSpike_t tspike = rawToTSpike(rdp);
  dataCache_.push_back(tspike); 
  if (dataCache_.size() == CACHESIZE -1)
    {
      flush(); 
    }

}

void TSpikeTable::updateName()
{
  /*
    reload our name from the dataset by reexamining the path. 
    

  */
  std::string path =  h5helper::getPath(selfDataSet_); 
  std::string name = h5helper::extractName(path); 
  tableName_ = name; 

}

void TSpikeTable::setSourceAttribute(H5::DataSet ds, datasource_t src)
{
  // Static function that sets the source


  H5::DataSpace attrspace; 

  // set source attribute
  H5::Attribute source = ds.createAttribute("src", 
					    H5::PredType::NATIVE_INT,
					    attrspace); 
  int isrc = src; 
  source.write(H5::PredType::NATIVE_INT, &isrc);   

}

datasource_t TSpikeTable::getSourceAttribute(H5::DataSet ds) 
{

  // set source attribute
  H5::Attribute source = ds.openAttribute("src"); 
  
  int isrc; 
  source.read(H5::PredType::NATIVE_INT, &isrc);   

  return isrc; 
}

void TSpikeTable::flush()
{
  // Flush all data to the table and clear the cache

  if (dataCache_.size() > 0) {
    
    H5TBappend_records(tableLoc_.getLocId(), tableName_.c_str(), 
		       dataCache_.size(), 
		       sizeof(TSpike_t), 
		       &dstOffsets_[0], 
		       &dstSizes_[0], 
		       &dataCache_[0]); 
    dataCache_.clear(); 
  }
}

TSpikeTable::~TSpikeTable()
{
  flush(); 
}



std::string TSpikeTable::getPath()
{
  /*
    Return the full path of our dataset


  */
  return h5helper::getPath(selfDataSet_); 
  
}

void TSpikeTable::appendStats(const SinkStats & stats)
{
  /*  
    Append another set of Sink Statistics to the current set
    of attributes
  */

  
  sinkStatsCache_.push_back(stats); 
  
  
  h5helper::writeStatsAttribute(selfDataSet_, sinkStatsCache_); 
  
  
}

std::vector<SinkStats> TSpikeTable::getStats()
{
  return sinkStatsCache_; 
  
}
