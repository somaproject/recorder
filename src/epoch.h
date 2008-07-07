#ifndef EPOCH_H
#define EPOCH_H

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
      somatime_t startts; 
      int starttime; 
      somatime_t endtts; 
      int endtime; 
    };
    
    typedef std::pair< datasource_t, datatype_t> dpair_t; 
    
    class Epoch : boost::noncopyable
    {
      // abstract base class 
    public: 
      Epoch(epochname_t name); 

      epochname_t getName();
      
      void startRecording(); 
      void stopRecording(); 
      
      // data sink management 
      virtual void setDataSinkName(datasource_t src, string name); 
      virtual void enableDataSink(datasource_t src, 
				  datatype_t typ); 
      virtual void disableDataSink(datasource_t src, datatype_t typ); 

      virtual std::string getDataSinkName(datasource_t src, datatype_t typ); 

      virtual std::list<dpair_t> getAllDataSinks(); 

      // data appending
      virtual void appendData(const pDataPacket_t);
      
      // event management
      virtual void appendEvent(const pEventPacket_t );
      
      // Notes interface
      virtual pNote_t createNote(std::string name, std::string text); 
      virtual std::vector<pNote_t> getAllNotes(); 
      virtual void delNote(pNote_t); 

      // session interface; not entirely happy with this, will take more thought
      virtual std::list<Session> getSessions(); 
      virtual void addSession();   
      
      virtual ~Epoch() {}; 

    private:
      epochname_t name; 
      
    };
    
    typedef boost::shared_ptr<Epoch>  pEpoch_t; 
    
  }
}

#endif // EPOCHINTERFACE_H
