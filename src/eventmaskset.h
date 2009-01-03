#ifndef EVENTMASKSET_H
#define EVENTMASKSET_H

#include <somanetwork/event.h>
#include <boost/shared_ptr.hpp>
#include <vector>

/*
  A simple "set" of event (src, cmd) pairs that we can perform 
  set-like operations on but that is efficient. 
  
*/

namespace soma {
  namespace recorder {
    class EventMaskSet {
      
    public:
      EventMaskSet(); 
      ~EventMaskSet(); 
      
      bool isSet(eventsource_t, eventcmd_t) const; 
      void set(eventsource_t, eventcmd_t, bool state); 
      void set(eventsource_t, const std::vector<eventcmd_t> & , bool state); 
      
      std::vector<eventcmd_t> getSrcCmds(eventsource_t); 
      
      
      void allset(bool state); 

      static const int EVENT_SRC_MAX=79; 
      static const int  EVENT_CMD_MAX=255; 

    private:
      
      char*  pMaskLUT_; 
  
    }; 
    

  }

}


#endif // EVENTMASKSET_H
