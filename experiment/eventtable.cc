#include <boost/format.hpp> 
#include <iostream>
#include <somanetwork/event.h>
#include "eventtable.h"
#include "eventmaskset.h"

using namespace soma::recorder; 

const std::string EventTable::datasetName("Events"); 

pEventTable_t EventTable::open(H5::Group containingGroup) {

  try {
    pEventTable_t pEventTable(new EventTable(containingGroup)); 

    pEventTable->selfDataSet_ = containingGroup.openDataSet(datasetName); 
    
  // load the eventset
    H5::Attribute attr = pEventTable->selfDataSet_.openAttribute("eventmask"); 
    
    char eventset[EventMaskSet::EVENT_SRC_MAX + 1][EventMaskSet::EVENT_CMD_MAX + 1]; 
    
    attr.read(H5::PredType::NATIVE_CHAR, eventset); 
    
    for (int i = 0; i < EventMaskSet::EVENT_SRC_MAX + 1; i++) {
      for (int j = 0; j < EventMaskSet::EVENT_CMD_MAX + 1; j++) {
	pEventTable->eventMaskSet_.set(i, j, eventset[i][j] ); 
      }
    }
    pEventTable->isClosed_ = false; 
    return pEventTable; 
  } catch (H5::Exception & e) {
    throw std::runtime_error("Error opening Epoch table in file; bad file?"); 
  }

}

pEventTable_t EventTable::create(H5::Group containingGroup) {

  // instantiate the object

  pEventTable_t pEventTable(new EventTable(containingGroup)); 


  // Create the table 
  size_t SeqEvent_dst_size =  sizeof( SeqEvent_t );


  herr_t     status;

  hsize_t edata_dims[1] = {EVENTLEN-1}; 
  hid_t edatat = H5Tarray_create(H5T_NATIVE_UINT16, 1, edata_dims);  
  
  const char * SeqEvent_field_names[NFIELDS] = 
    { "seq", "cmd", "src", "data"}; 
 
  hid_t SeqEvent_field_type[NFIELDS]; 
  SeqEvent_field_type[0] = H5T_NATIVE_UINT32; 
  SeqEvent_field_type[1] = H5T_NATIVE_UINT8; 
  SeqEvent_field_type[2] = H5T_NATIVE_UINT8; 
  SeqEvent_field_type[3] = edatat; 

  
  status = H5TBmake_table( datasetName.c_str(), 
			   pEventTable->tableLoc_.getLocId(),
			   pEventTable->tableName_.c_str(), 
			   NFIELDS, 
			   0, 
			   SeqEvent_dst_size, 
			   SeqEvent_field_names, 
			   &(pEventTable->dstOffsets_[0]), 
			   SeqEvent_field_type,
			   100, NULL, 
			   false, NULL  );

  pEventTable->selfDataSet_ = containingGroup.openDataSet(datasetName); 

  pEventTable->isClosed_ = false; 
  
  return pEventTable; 
  
}





EventTable::EventTable(H5::Group containingGroup) :
  seqeventCache_(),
  tableLoc_(containingGroup), 
  eventMaskSet_(), 
  isClosed_(true)
{
  // setup cache
  seqeventCache_.reserve(CACHESIZE); 
  tableName_ = "Events"; 
  
  setupH5Types(); 
}

void EventTable::setupH5Types()
{
  
  dstOffsets_.push_back(HOFFSET( SeqEvent_t, seq)); 
  dstOffsets_.push_back(HOFFSET( SeqEvent_t, cmd)); 
  dstOffsets_.push_back(HOFFSET( SeqEvent_t, src)); 
  dstOffsets_.push_back(HOFFSET( SeqEvent_t, data)); 

  SeqEvent_t tests; 
  dstSizes_.push_back(sizeof( tests.seq)); 
  dstSizes_.push_back(sizeof( tests.cmd)); 
  dstSizes_.push_back(sizeof( tests.src)); 
  dstSizes_.push_back(sizeof( tests.data)); 


}
void EventTable::append(const SeqEvent_t & se)
{
  
  
  seqeventCache_.push_back(se); 
  
  if (seqeventCache_.size() == CACHESIZE -1)
    {
      flush(); 
    }
  
  
}

void EventTable::flush()
{
  if (seqeventCache_.size() > 0) {
    
    H5TBappend_records(tableLoc_.getLocId(), tableName_.c_str(), 
		       seqeventCache_.size(), 
		       sizeof(SeqEvent_t), 
		       &dstOffsets_[0], 
		       &dstSizes_[0], 
		       &seqeventCache_[0]); 
    seqeventCache_.clear(); 
  }
}

void EventTable::close() {
  saveEventMaskSet(eventMaskSet_);
  flush(); 
  isClosed_ = true; 

}

EventTable::~EventTable()
{

  if (!isClosed_) {
    close(); 
  }
}



void EventTable::saveEventMaskSet(const EventMaskSet& ems)
{
  // FIXME
  const int RANK = 2; 
  hsize_t dim[] = {EventMaskSet::EVENT_SRC_MAX + 1, 
		   EventMaskSet::EVENT_CMD_MAX + 1}; 
  H5::DataSpace space( RANK, dim );

  H5::Attribute masksetattr; 
  bool exists = false; 
  
  for (int i = 0; i < selfDataSet_.getNumAttrs();  ++i) {
    H5::Attribute a = selfDataSet_.openAttribute(i); 
    if (a.getName() == "eventmask") {
      exists = true; 
      masksetattr=a; 
    }
    
  }
  if (!exists) {
    masksetattr = 
      selfDataSet_.createAttribute("eventmask", H5::PredType::NATIVE_CHAR, space);
  }
  

  
  char eventset[EventMaskSet::EVENT_SRC_MAX + 1][EventMaskSet::EVENT_CMD_MAX + 1]; 
  for (int i = 0; i < EventMaskSet::EVENT_SRC_MAX + 1; i++) {
    for (int j = 0; j < EventMaskSet::EVENT_CMD_MAX + 1; j++) {
      eventset[i][j] = ems.isSet(i, j); 
    }
  }
  
  masksetattr.write(H5::PredType::NATIVE_CHAR, eventset); 


}


void EventTable::addEventRXMask(eventsource_t src, const std::vector<eventcmd_t> & cmds) 
{
  for(int i = 0; i < cmds.size(); i++) {
    eventMaskSet_.set(src, cmds[i], true); 
  }
  
}

std::vector<eventcmd_t> EventTable::getEventRXMask(eventsource_t src) 
{
  
  std::vector<eventcmd_t> results; 
  results.reserve(EventMaskSet::EVENT_CMD_MAX+1); 

  for(int i = 0; i < EventMaskSet::EVENT_CMD_MAX+1; i++) {
    if (eventMaskSet_.isSet(src, i)) {
      results.push_back(i); 
    }
  }
  return results; 
}

void EventTable::removeEventRXMask(eventsource_t src, const std::vector<eventcmd_t> & cmds)
{

  for(int i = 0; i < cmds.size(); i++) {
    eventMaskSet_.set(src, cmds[i], false); 
  }
  
}

