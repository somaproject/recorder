#ifndef H5EPOCH_H
#define H5EPOCH_H

#include <map>
#include <boost/utility.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <H5Cpp.h>
 
#include "epochinterface.h"
#include "datasetio.h"


namespace soma {
  namespace recorder {
    typedef boost::ptr_map<dpair_t, DatasetIO> dispatchTable_t; 
    typedef std::map<dpair_t, std::string> namedSinkTable_t; 
    
    
    class H5Epoch : public EpochInterface
      {
      public:
	H5Epoch(H5::Group g); 
	H5Epoch(H5::Group g, epochname_t name);  // is this constructor necessary?
	
	//------------------------------------------------------
	//  Epoch Interface
	//------------------------------------------------------
	
	epochname_t getName();
	
	// data sink management 
	void enableDataSink(datasource_t src, 
			    datatype_t typ, std::string name); 
	
 	void disableDataSink(datasource_t src, datatype_t typ); 
	
	std::string setDataSinkName(datasource_t src, datatype_t typ, 
				    std::string name); 
	std::string getDataSinkName(datasource_t src, datatype_t typ);  
	
	std::list<dpair_t> getAllDataSinks(); 
	
	void appendData(const pDataPacket_t); 
  
	// event management (FIXME)
	
	// Notes interface
	void appendNote(std::string note) { };  
	std::list<Notes> getNotes() { };  
  
	// session interface; not entirely happy with this, will take more thought
	std::list<Session> getSessions() { };  
	void addSession() { };    
	
	
      private:
	H5::Group h5group_; // internal group
	epochname_t name_; 
	
	H5::Group getTypeGroup(datatype_t typ); 
	H5::Group getEventGroup(); 

	dispatchTable_t dispatchTable_; 
	namedSinkTable_t namedSinkTable_; 
	
	
      }; 
    
  }
}

#endif // H5EPOCH_H
