#ifndef EVENTTABLE_H
#define EVENTTABLE_H

#include <vector>

#include "H5Cpp.h"
#include "hdf5_hl.h"
#include <somanetwork/event.h>

using namespace H5;

namespace soma {
  namespace recorder {
    
    struct SeqEvent_t { 
      // sequence number + event; for later debugging and dropped
      // packet detection
      eventseq_t seq; 
      eventcmd_t cmd; 
      eventsource_t src; 
      boost::array<uint16_t, EVENTLEN-1> data;

    }; 

    class EventTable
      {
	static const int CACHESIZE =100; 
	
      public:
	EventTable(H5::Group gloc); 
	~EventTable(); 
	void append(pEventPacket_t ); 
	void flush();
	
      private:
	int src_; 
	std::vector<SeqEvent_t> seqeventCache_; 
	H5::Group tableLoc_; 
	std::vector<size_t> dstSizes_; 
	std::vector<size_t> dstOffsets_; 
	std::string tableName_; 

	//size_t fieldTypes[]; 
	
      }; 
  }
}


#endif // WAVETABLE_H
