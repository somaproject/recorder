#include <boost/format.hpp> 
#include <iostream>
#include <somanetwork/wave.h>
#include <somanetwork/datapacket.h>

#include "wavetable.h"

using namespace soma::recorder; 

WaveTable::WaveTable(datasource_t src, std::string name, H5::Group gloc) :
  src_(src), 
  dataCache_(),
  tableLoc_(gloc), 
  tableName_(name)
{
  // setup cache
  dataCache_.reserve(CACHESIZE); 

  // construct the table

  const int NFIELDS = 6; 
  size_t Wave_dst_size =  sizeof( Wave_t );
  
  dstOffsets_.push_back(HOFFSET( Wave_t, src)); 
  dstOffsets_.push_back(HOFFSET( Wave_t, time)); 
  dstOffsets_.push_back(HOFFSET( Wave_t, samprate)); 
  dstOffsets_.push_back(HOFFSET( Wave_t, selchan)); 
  dstOffsets_.push_back(HOFFSET( Wave_t, filterid)); 
  dstOffsets_.push_back(HOFFSET( Wave_t, wave)); 


  Wave_t tests; 
  dstSizes_.push_back(sizeof( tests.src)); 
  dstSizes_.push_back(sizeof( tests.time)); 
  dstSizes_.push_back(sizeof( tests.samprate)); 
  dstSizes_.push_back(sizeof( tests.selchan)); 
  dstSizes_.push_back(sizeof( tests.filterid)); 
  dstSizes_.push_back(sizeof( tests.wave)); 

  herr_t     status;

  hsize_t wave_dims[1] = {WAVEBUF_LEN}; 
  hid_t wt = H5Tarray_create(H5T_NATIVE_INT32, 1, wave_dims);  

  const char * Wave_field_names[NFIELDS] = 
    { "src", "time", "samprate", "selchan", "filterid", "wave" }; 
  
  hid_t Wave_field_type[NFIELDS]; 
  Wave_field_type[0] = H5T_NATIVE_UINT8; 
  Wave_field_type[1] = H5T_NATIVE_UINT64; 
  Wave_field_type[2] = H5T_NATIVE_UINT16; 
  Wave_field_type[3] = H5T_NATIVE_UINT16; 
  Wave_field_type[4] = H5T_NATIVE_UINT16; 
  Wave_field_type[5] = wt; 


  
  status = H5TBmake_table( "Table Title", 
			   tableLoc_.getLocId(),
			   tableName_.c_str(), 
			   NFIELDS, 
			   0, 
			   Wave_dst_size, 
			   Wave_field_names, 
			   &dstOffsets_[0], 
			   Wave_field_type,
			   100, NULL, 
			   false, NULL  );
  
}

void WaveTable::append(pDataPacket_t rdp)
{
  Wave_t wave = rawToWave(rdp);
  dataCache_.push_back(wave); 
  if (dataCache_.size() == CACHESIZE -1)
    {
      flush(); 
    }

}

void WaveTable::flush()
{
  if (dataCache_.size() > 0) {
    
    H5TBappend_records(tableLoc_.getLocId(), tableName_.c_str(), 
		       dataCache_.size(), 
		       sizeof(Wave_t), 
		       &dstOffsets_[0], 
		       &dstSizes_[0], 
		       &dataCache_[0]); 
    dataCache_.clear(); 
  }
}

WaveTable::~WaveTable()
{
  flush(); 
}
