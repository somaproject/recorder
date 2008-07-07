#ifndef H5EPOCH_H
#define H5EPOCH_H

#include <map>
#include <boost/utility.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <H5Cpp.h>
 
#include "epoch.h"
#include "datasetio.h"
#include "eventtable.h"
#include "notetable.h" 



namespace soma {
  namespace recorder {

    typedef boost::ptr_map<dpair_t, DatasetIO> dispatchTable_t; 
    typedef std::map<dpair_t, std::string> namedSinkTable_t; 
    
    
    class H5Epoch : public Epoch
      {
      public:
	H5Epoch(H5::Group g); // how do we load? 
	H5Epoch(H5::Group g, epochname_t name);  // is this constructor necessary?
	~H5Epoch(); 
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
  
	// event management
	void appendEvent(const pEventPacket_t); 
	
	// Notes interface
	void createNote(std::string name, std::string text); 
	void setNote(std::string name, std::string text); 
	Note_t getNote(std::string name); 
	void deleteNote(std::string name); 
	std::list<Note_t> getAllNotes(); 

  
	// session interface; not entirely happy with this, will take more thought
	std::list<Session> getSessions() { };  
	void addSession() { };    
	
	
      private:
	H5::Group h5group_; // internal group
	epochname_t name_; 
	
	H5::Group getTypeGroup(datatype_t typ); 

	dispatchTable_t dispatchTable_; 
	namedSinkTable_t namedSinkTable_; 

	pEventTable_t pEventTable_; 
	pEventTable_t openEventTable(); 

	pNoteTable_t pNoteTable_; 
	pNoteTable_t openNoteTable(); 

      }; 
    
  }
}

#endif // H5EPOCH_H
