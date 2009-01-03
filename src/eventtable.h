#ifndef EVENTTABLE_H
#define EVENTTABLE_H

#include <vector>

#include "H5Cpp.h"
#include "hdf5_hl.h"
#include <somanetwork/event.h>
#include "eventmaskset.h"

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

    class EventTable; 
    typedef boost::shared_ptr<EventTable> pEventTable_t; 

    class EventTable
      {
	static const int CACHESIZE =100; 
	static const std::string datasetName; 

      public:
	static pEventTable_t open(H5::Group containingGroup); 
	static pEventTable_t create(H5::Group containingGroup); 
	~EventTable(); 
	void append(const SeqEvent_t & es); 
	void flush();
	void close(); 

	EventMaskSet eventMaskSet_; 

	void addEventRXMask(eventsource_t src, 
			    const std::vector<eventcmd_t> & cmds); 
	std::vector<eventcmd_t> getEventRXMask(eventsource_t src); 
	void removeEventRXMask(eventsource_t src, const std::vector<eventcmd_t> & cmds); 
	
      private:
	EventTable(H5::Group gloc); 
	int src_; 
	std::vector<SeqEvent_t> seqeventCache_; 
	bool isClosed_; 
	H5::Group tableLoc_; 
	std::vector<size_t> dstSizes_; 
	std::vector<size_t> dstOffsets_; 
	std::string tableName_; 
	void setupH5Types(); 

	H5::DataSet selfDataSet_; 
	void saveEventMaskSet(const EventMaskSet& ems); 

	//size_t fieldTypes[]; 
	static const int NFIELDS = 4; 
      }; 
    
    typedef boost::shared_ptr<EventTable> pEventTable_t; 
  }
}


#endif // EVENTTABLE_H
