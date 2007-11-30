#include <boost/format.hpp> 
#include <iostream>
#include <somanetwork/event.h>
#include "eventtable.h"

using namespace soma::recorder; 

EventTable::EventTable(H5::Group gloc) :
  seqeventCache_(),
  tableLoc_(gloc)
{
  // setup cache
  seqeventCache_.reserve(CACHESIZE); 
  tableName_ = "events"; 

  // construct the table

  const int NFIELDS = 4; 
  size_t SeqEvent_dst_size =  sizeof( SeqEvent_t );
  
  dstOffsets_.push_back(HOFFSET( SeqEvent_t, seq)); 
  dstOffsets_.push_back(HOFFSET( SeqEvent_t, cmd)); 
  dstOffsets_.push_back(HOFFSET( SeqEvent_t, src)); 
  dstOffsets_.push_back(HOFFSET( SeqEvent_t, data)); 

  //dstOffsets_.push_back(HOFFSET( SeqEvent_t, data)); 
  
  
  
  SeqEvent_t tests; 
  dstSizes_.push_back(sizeof( tests.seq)); 
  dstSizes_.push_back(sizeof( tests.cmd)); 
  dstSizes_.push_back(sizeof( tests.src)); 
  dstSizes_.push_back(sizeof( tests.data)); 

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


  
  status = H5TBmake_table( "Events", 
			   tableLoc_.getLocId(),
			   tableName_.c_str(), 
			   NFIELDS, 
			   0, 
			   SeqEvent_dst_size, 
			   SeqEvent_field_names, 
			   &dstOffsets_[0], 
			   SeqEvent_field_type,
			   100, NULL, 
			   false, NULL  );
  
}

void EventTable::append(pEventPacket_t pel)
{
  // extract out the events as SeqEvents; 
  if (not pel->missing) {
    for (EventList_t::iterator e = pel->events->begin(); 
	 e != pel->events->end(); ++e) {
      SeqEvent_t se; 
      se.seq = pel->seq; 
      se.cmd = e->cmd; 
      se.src = e->src; 
      se.data = e->data; 
      seqeventCache_.push_back(se); 

      if (seqeventCache_.size() == CACHESIZE -1)
	{
	  flush(); 
	}
      
      
    }
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

EventTable::~EventTable()
{
  flush(); 
}
