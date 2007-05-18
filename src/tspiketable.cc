#include <boost/format.hpp> 
#include <iostream>
#include <network/data/tspike.h>
#include <network/data/rawdata.h>


#include "tspiketable.h"
TSpikeTable::TSpikeTable(datasource_t src, H5::Group gloc) :
  src_(src), 
  dataCache_(),
  tableLoc_(gloc)
{
  // setup cache
  dataCache_.reserve(CACHESIZE); 

  // construct the table

  const int NFIELDS = 6; 
  size_t TSpike_dst_size =  sizeof( TSpike_t );
  
  dstOffsets_.push_back(HOFFSET( TSpike_t, src)); 
  dstOffsets_.push_back(HOFFSET( TSpike_t, time)); 
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

  
  hsize_t wave_dims[1] = {TSPIKEWAVE_LEN}; 
  hid_t wt = H5Tarray_create(H5T_NATIVE_INT32, 1, wave_dims, NULL);  

  hid_t TSpikeWave_type = H5Tcreate(H5T_COMPOUND, sizeof(TSpikeWave_t)); 
  H5Tinsert(TSpikeWave_type, "filtid", 
	    HOFFSET(TSpikeWave_t, filtid), H5T_NATIVE_UINT8); 
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

  tableName_ = str(boost::format("chan%1d") % src); 
  
  status = H5TBmake_table( "Table Title", 
			   tableLoc_.getLocId(),
			   tableName_.c_str(), 
			   NFIELDS, 
			   0, 
			   TSpike_dst_size, 
			   TSpike_field_names, 
			   &dstOffsets_[0], 
			   TSpike_field_type,
			   100, NULL, 
			   false, NULL  );
  
}

void TSpikeTable::append(const RawData * rdp)
{
  TSpike_t tspike = rawToTSpike(rdp);
  dataCache_.push_back(tspike); 
  if (dataCache_.size() == CACHESIZE -1)
    {
      flush(); 
    }

}

void TSpikeTable::flush()
{
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
