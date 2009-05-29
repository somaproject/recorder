#ifndef H5EXPERIMENT_H
#define H5EXPERIMENT_H

#include <map>
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/utility.hpp>
#include <boost/filesystem/operations.hpp>
#include <somanetwork/network.h>
#include <somanetwork/datapacket.h>
#include <somanetwork/event.h>

#include <H5Cpp.h>
#include <gtkmm/main.h>

#include "note.h"
#include "h5epoch.h"
#include "notetable.h"
#include "logging.h"
#include "experiment.h"

namespace soma {
  namespace recorder {
    class H5Epoch; 
    class H5Experiment; 
    typedef boost::shared_ptr<H5Experiment> pH5Experiment_t; 
    typedef boost::weak_ptr<H5Experiment> pH5ExperimentWeak_t; 

    class H5Experiment : public Experiment,  
			 public boost::enable_shared_from_this<H5Experiment>
    {
      typedef boost::filesystem::path filename_t; 
    public:

      static pExperiment_t create(pNetworkInterface_t pn, filename_t expfilename); 
      
      static pExperiment_t open(pNetworkInterface_t pn, filename_t expfilename); 
      
      std::map<string, string> getFileProperties(); 
      
      // data dispatch, so that we can externally interface to the 
      // network object. Thus we can be single-threaded
      void dispatchData(pDataPacket_t dp); 
      void dispatchEvent(pEventPacket_t ep); 
      
      std::string getName(); 

      // epoch interface
      std::vector<pEpoch_t> getEpochs(); 
      pEpoch_t createEpoch(epochname_t name); 
      void deleteEpoch(pEpoch_t); 
      pEpoch_t getEpoch(epochname_t name); 
      void renameEpoch(pEpoch_t epoch, epochname_t name); 

      void close(); 

      //---------------------------------------------------
      //  note management
      //---------------------------------------------------
      
      notehandle_t createNote(); 
      Note_t getNote(notehandle_t); 
      std::list<notehandle_t> getNotes(); 
      void setNote(const Note_t & note); 
      void deleteNote(notehandle_t); 
      
      

      ~H5Experiment(); 

      void startRecording(epochname_t targetEpoch); 
      void stopRecording(); 
      
      somatime_t currentTS_; 
      int currentTime_; 
      
    private:
      H5Experiment(pNetworkInterface_t pn, H5::H5File file); 

      H5::H5File h5file_; 
      filename_t filename_; 
      pNetworkInterface_t pNetwork_; 
      typedef std::map<epochname_t, boost::shared_ptr<H5Epoch> > epochmap_t; 
      epochmap_t epochs_; 

      typedef std::map<int, boost::shared_ptr<H5Epoch> > epochordermap_t;  
      epochordermap_t epochsOrdered_; 
      
      boost::shared_ptr<H5Epoch> currentTargetEpoch_; 

      int getNextEpochOrderID(); 

      bool isRecording_; 
      
      pEpoch_t setupEpoch(H5::Group epochgroup); 

      somatime_t getCurrentTS(); 
      void setCurrentTS(somatime_t); 

      void setCurrentTime(int); 
      int getCurrentTime(); 
      pNoteTable_t pNoteTable_; 
      static std::map<dpair_t, SinkStats> 
      convertStats(const std::vector<DataReceiverStats> & stats); 

      // Network callbacks
      bool dataRXCallback(Glib::IOCondition io_condition); 
      bool eventRXCallback(Glib::IOCondition io_condition); 
      
      log4cpp::Category& logexperiment_; 
      
      bool isClosed_; 

    };
    
  }
}





#endif // H5EXPERIMENT_H
