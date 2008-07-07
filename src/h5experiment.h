#ifndef H5EXPERIMENT_H
#define H5EXPERIMENT_H

#include <map>
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <somanetwork/network.h>
#include <somanetwork/datapacket.h>
#include <somanetwork/event.h>
#include "note.h"
#include "epoch.h"
#include "experiment.h"

namespace soma {
  namespace recorder {
    class H5Experiment : public Experiment {
      
    public:
      createExperiment(std::string somaIP, std::string expfilename); 
      openExperiment(std::string somaIP, std::string expfilename); 
      
      // epoch interface
      std::vector<pEpoch_t> getEpochs(); 
      void createEpoch(epochname_t name); 
      void deleteEpoch(pEpoch_t); 
      pEpoch_t getEpoch(epochname_t name); 
    
      // state manipulation interface
          // default data sink manipulation
      void setDefaultDataState(datasource_t src, bool isAvailable, 
			       bool isEnabled, string name); 
      
      void getDefaultDataState(); 
      

      void close(); 
      
    private:
      H5Experiment(std::string somaip, std::string filename);
      ~H5Experiment(); 
      
      
  }
}





#endif // H5EXPERIMENT_H
