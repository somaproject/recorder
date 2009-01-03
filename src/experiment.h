#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <map>
#include <string>
#include <list>
#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <somanetwork/network.h>
#include <somanetwork/datapacket.h>
#include <somanetwork/event.h>
#include "note.h"
#include "epoch.h"
#include "noteinterface.h"

namespace soma { 
  namespace recorder { 
    
    class Experiment; 
    
    typedef boost::shared_ptr<Experiment> pExperiment_t; 
    class Experiment : public NoteInterface{
      
      
    public:
      // exposed methods
      virtual std::map<string, string> getFileProperties() = 0;  
      virtual std::string getName() = 0; 
      // Epoch Manipulation
      virtual std::vector<pEpoch_t> getEpochs()  = 0;
      virtual pEpoch_t createEpoch(epochname_t name)  = 0;
      virtual void deleteEpoch(pEpoch_t)  = 0;
      virtual pEpoch_t getEpoch(epochname_t name)  = 0;
      virtual void renameEpoch(pEpoch_t epoch, epochname_t name)  = 0;
      
      // data dispatch
      virtual void dispatchData(pDataPacket_t dp) = 0; 
      virtual void dispatchEvent(pEventPacket_t el) =0; 
      
      
      virtual void close()  = 0;   
      
    }; 

  }
}


#endif // EXPERIMENT_H
