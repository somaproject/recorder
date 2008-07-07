#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <map>
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <somanetwork/datapacket.h>
#include <somanetwork/event.h>
#include "note.h"
#include "epoch.h"

namespace soma
{
  
  class Experiment {
    

  public:
    Experiment(); 

    // exposed methods
    std::map<string, string> getFileProperties(); 

    // Epoch Manipulation
    virtual std::vector<pEpoch_t> getEpochs(); 
    virtual void createEpoch(epochname_t name); 
    virtual void deleteEpoch(pEpoch_t); 
    virtual pEpoch_t  getEpoch(epochname_t name); 
    
    // note manipulation
    void createNote(string title, string text); 
    std::vector<pNote_t> getNotes(); 
    
    // default data sink manipulation
    void setDefaultDataState(char src, bool isAvailable, 
			     bool isEnabled, string name); 

    void getDefaultDataState(); 

    // how to get data state? 
    
    void close(); 
    
    
  private:
    std::vector<pEpoch_t> epochs_; 
    
    void startRecording(pEpoch_t targetEpoch); 
    void stopRecording(); 

    bool isRecording_; 
    

  }; 


}


#endif // EXPERIMENT_H
