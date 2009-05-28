#include <boost/format.hpp> 
#include <iostream>
#include <somanetwork/wave.h>
#include <somanetwork/datapacket.h>
#include "h5helper.h"
#include "wavetable.h"

using namespace soma::recorder; 
pWaveTable_t WaveTable::create(H5::Group containingGroup, std::string name, 
			       datasource_t src)
{
  pWaveTable_t pwt(new WaveTable(name, containingGroup)); 


  hsize_t wave_dims[1] = {WAVEBUF_LEN}; 
  hid_t wt = H5Tarray_create(H5T_NATIVE_INT32, 1, wave_dims);  

  size_t Wave_dst_size =  sizeof( Wave_t );

  const char * Wave_field_names[NFIELDS] = 
    { "src", "time", "sampratenum", "samprateden", 
      "selchan", "filterid", "wave" }; 
  
  hid_t Wave_field_type[NFIELDS]; 
  Wave_field_type[0] = H5T_NATIVE_UINT8; 
  Wave_field_type[1] = H5T_NATIVE_UINT64; 
  Wave_field_type[2] = H5T_NATIVE_UINT16; 
  Wave_field_type[3] = H5T_NATIVE_UINT16; 
  Wave_field_type[4] = H5T_NATIVE_UINT16; 
  Wave_field_type[5] = H5T_NATIVE_UINT32; 
  Wave_field_type[6] = wt; 

  // construct the table

  herr_t     status;

  status = H5TBmake_table( "Table Title", 
			   pwt->tableLoc_.getLocId(),
			   pwt->tableName_.c_str(), 
			   NFIELDS, 
			   0, 
			   Wave_dst_size, 
			   Wave_field_names, 
			   &(pwt->dstOffsets_[0]), 
			   Wave_field_type,
			   100, NULL, 
			   false, NULL  );

  pwt->src_ = src; 
  pwt->selfDataSet_ = pwt->tableLoc_.openDataSet(pwt->tableName_); 
  setSourceAttribute(pwt->selfDataSet_, src); 

  return pwt; 
}
	
pWaveTable_t WaveTable::open(H5::Group containingGroup, std::string name) {

  pWaveTable_t pwt(new WaveTable(name, containingGroup)); 
  
  
  pwt->selfDataSet_ = pwt->tableLoc_.openDataSet(pwt->tableName_); 
  pwt->src_ = getSourceAttribute(pwt->selfDataSet_); 
  return pwt; 

}

WaveTable::WaveTable(std::string name, H5::Group gloc) :
  dataCache_(),
  tableLoc_(gloc), 
  tableName_(name)
{
  // setup cache
  dataCache_.reserve(CACHESIZE); 
  setupDataTypes(); 

}

void WaveTable::updateName()
{
  /*
    reload our name from the dataset by reexamining the path. 
    

  */
  std::string path =  h5helper::getPath(selfDataSet_); 
  std::string name = h5helper::extractName(path); 
  tableName_ = name; 

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

void WaveTable::setupDataTypes()
{

  dstOffsets_.push_back(HOFFSET( Wave_t, src)); 
  dstOffsets_.push_back(HOFFSET( Wave_t, time)); 
  dstOffsets_.push_back(HOFFSET( Wave_t, sampratenum)); 
  dstOffsets_.push_back(HOFFSET( Wave_t, samprateden)); 
  dstOffsets_.push_back(HOFFSET( Wave_t, selchan)); 
  dstOffsets_.push_back(HOFFSET( Wave_t, filterid)); 
  dstOffsets_.push_back(HOFFSET( Wave_t, wave)); 


  Wave_t tests; 
  dstSizes_.push_back(sizeof( tests.src)); 
  dstSizes_.push_back(sizeof( tests.time)); 
  dstSizes_.push_back(sizeof( tests.sampratenum)); 
  dstSizes_.push_back(sizeof( tests.samprateden)); 
  dstSizes_.push_back(sizeof( tests.selchan)); 
  dstSizes_.push_back(sizeof( tests.filterid)); 
  dstSizes_.push_back(sizeof( tests.wave)); 



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

void WaveTable::setSourceAttribute(H5::DataSet ds, datasource_t src)
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

datasource_t WaveTable::getSourceAttribute(H5::DataSet ds) 
{

  // set source attribute
  H5::Attribute source = ds.openAttribute("src"); 
  
  int isrc; 
  source.read(H5::PredType::NATIVE_INT, &isrc);   

  return isrc; 
}


std::string WaveTable::getPath()
{
  /*
    Return the full path of our dataset


  */
  return h5helper::getPath(selfDataSet_); 
  
}

void WaveTable::appendStats(const SinkStats & stats)
{
  /*  
    Append another set of Sink Statistics to the current set
    of attributes
  */

  sinkStatsCache_.push_back(stats); 
  h5helper::writeStatsAttribute(selfDataSet_, sinkStatsCache_); 
  
}

std::vector<SinkStats> WaveTable::getStats()
{
  return sinkStatsCache_; 
  
}
