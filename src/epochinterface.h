#ifndef EPOCHINTERFACE_H
#define EPOCHINTERFACE_H

#include <string>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <somanetwork/datapacket.h>
#include <somanetwork/event.h>
#include "note.h"

namespace soma {
  namespace recorder {
    
    typedef std::string epochname_t; 
    
    struct Session { 
      // starttime 
      // starttime ts
      // end time
      // endtime ts
      
    };
    
    typedef std::pair< datasource_t, datatype_t> dpair_t; 
    
    
    class EpochInterface : boost::noncopyable
    {
      // abstract base class 
    public: 
      virtual epochname_t getName() = 0; 
      
      // data sink management 
      virtual void enableDataSink(datasource_t src, 
				  datatype_t typ, std::string name) = 0; 
      virtual void disableDataSink(datasource_t src, datatype_t typ) = 0; 
      virtual std::string getDataSinkName(datasource_t src, datatype_t typ) = 0; 

      virtual std::list<dpair_t> getAllDataSinks() = 0; 
      
      virtual void appendData(const pDataPacket_t) = 0;
      
      // event management
      virtual void appendEvent(const pEventPacket_t ) = 0;
      
      // Notes interface
      virtual void createNote(std::string name, std::string text) = 0; 
      virtual void setNote(std::string name, std::string text) = 0; 
      virtual Note_t getNote(std::string name) = 0; 
      virtual void delNote(std::string name) = 0; 
      virtual std::list<Note_t> getAllNotes() = 0; 
      
      // session interface; not entirely happy with this, will take more thought
      virtual std::list<Session> getSessions() = 0; 
      virtual void addSession() = 0;   
      
      virtual ~EpochInterface() {}; 
    };
    
    typedef boost::shared_ptr<EpochInterface>  pEpochInterface_t; 
  }
}

#endif // EPOCHINTERFACE_H
