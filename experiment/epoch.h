#ifndef EPOCH_H
#define EPOCH_H

#include <string>
#include <list>
#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/utility.hpp>
#include <somanetwork/datapacket.h>
#include <somanetwork/event.h>

#include "noteinterface.h"

#include "sinkstats.h"


namespace soma {
  namespace recorder {
    using namespace somanetwork; 
    
    using namespace std;     
    
    typedef std::string epochname_t; 
    typedef uint64_t somatime_t; 
    struct Session { 
      somatime_t startts; 
      int starttime; 
      somatime_t endts; 
      int endtime; 
    };


    typedef std::pair< datasource_t, datatype_t> dpair_t; 
    
    class Epoch : boost::noncopyable, public NoteInterface
    {
      // abstract base class 
    public: 
      virtual epochname_t getName() =0;
      
      virtual void startRecording() =0; 
      virtual void stopRecording() =0; 
      virtual bool isRecording() = 0;
      // data sink interface
      
      virtual void enableDataSink(datasource_t src, datatype_t typ)  = 0;   
      virtual void disableDataSink(datasource_t src, datatype_t typ)  = 0;   
      virtual std::set<dpair_t> getDataSinks()  = 0;  

      virtual void setDataName(datasource_t src, std::string name)  = 0;   
      virtual std::string getDataName(datasource_t src)  = 0;   
      
      virtual std::vector<SinkStats> getSinkStatistics(datasource_t src,
						       datatype_t typ)  = 0; 

      // Event Management
      virtual void addEventRXMask(eventsource_t src, 
				  const std::vector<eventcmd_t> & cmds) = 0; 
      virtual std::vector<eventcmd_t> getEventRXMask(eventsource_t src) = 0; 
      virtual void removeEventRXMask(eventsource_t src, 
				     const std::vector<eventcmd_t> & cmds) = 0; 
      // data sink management 
      
      // data appending
      virtual void appendData(const pDataPacket_t) =0;
      
      // event management
      virtual void appendEvent(const pEventPacket_t ) =0;
      

      // session interface; not entirely happy with this, will take more thought
      virtual std::list<Session> getSessions() =0; 
      virtual void addSession() =0;   
      virtual std::map<std::string, std::string> 
      getSessionStatistics(int session)  = 0; 
      
      virtual ~Epoch() {}; 

    private:
      
    };
    
    typedef boost::shared_ptr<Epoch>  pEpoch_t; 
    
  }
}

#endif // EPOCHINTERFACE_H
