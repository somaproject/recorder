#ifndef H5EPOCH_H
#define H5EPOCH_H

#include <map>
#include <boost/utility.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <H5Cpp.h>
 
#include "h5experiment.h"
#include "epoch.h"
#include "datasetio.h"
#include "logging.h" 
#include "eventtable.h"
#include "notetable.h" 

#include "eventmaskset.h"



namespace soma {
  namespace recorder {

    typedef boost::ptr_map<dpair_t, pDatasetIO_t> dispatchTable_t; 
    typedef std::map<datasource_t, std::string> namedSinkTable_t; 
    typedef std::multimap<datasource_t, pDatasetIO_t> sourceSinkTable_t; 
    
    class H5Experiment; 
    typedef boost::weak_ptr<H5Experiment> pH5ExperimentWeak_t; 
    
    class H5Epoch; 
    typedef boost::shared_ptr<H5Epoch> pH5Epoch_t; 

    
    class H5Epoch : public Epoch
      {
      public:
	static pH5Epoch_t create(pH5ExperimentWeak_t parent, H5::Group containingGroup, 
				 std::string name,  int epochOrderID); 
	
	static pH5Epoch_t open(pH5ExperimentWeak_t parent, 
			       H5::Group epochGroup); 
	
		
	~H5Epoch(); 
	//------------------------------------------------------
	//  Epoch Interface
	//------------------------------------------------------
	
	epochname_t getName();
	
	void startRecording(); 
	void stopRecording(); 
	bool isRecording() { return isRecording_;}

	//---------------------------------------------------
	// data sink management 
	//---------------------------------------------------

	void enableDataSink(datasource_t src, datatype_t typ)  ;   
	void disableDataSink(datasource_t src, datatype_t typ)  ;   
	std::set<dpair_t> getDataSinks()  ;  
	void setDataName(datasource_t src, std::string name)  ;   
	std::string getDataName(datasource_t src)  ;   
	std::vector<SinkStats> getSinkStatistics(datasource_t src, datatype_t typ); 

	// add data
	void appendData(const pDataPacket_t); 

	//---------------------------------------------------
	//  event management
	//---------------------------------------------------

	// event interface
	void addEventRXMask(eventsource_t src, 
			    const std::vector<eventcmd_t> & cmds); 
	std::vector<eventcmd_t> getEventRXMask(eventsource_t src); 
	void removeEventRXMask(eventsource_t src, const std::vector<eventcmd_t> & cmds); 
	
	// event appending
	void appendEvent(pEventPacket_t); 
	
	//---------------------------------------------------
	//  note management
	//---------------------------------------------------

	notehandle_t createNote(); 
	Note_t getNote(notehandle_t); 
	std::list<notehandle_t> getNotes(); 
	void setNote(const Note_t & note); 
	void deleteNote(notehandle_t); 

	//---------------------------------------------------
	//  session management
	//---------------------------------------------------

	std::list<Session> getSessions();  
	void addSession() { };    

	std::map<std::string, std::string> 
	getSessionStatistics(int session); 
	
	int getEpochOrderID(); 	

	void startSession(somatime_t ts, int time); 
	void endSession(somatime_t ts, int time, const std::map<dpair_t, SinkStats> & stats ); 
	void setCurrentTS(somatime_t ts); 
      private:
	
	H5Epoch(pH5ExperimentWeak_t parent, H5::Group epochgroup);  

	H5::Group h5group_; // internal group
	
	H5::Group getTypeGroup(datatype_t typ); 
	bool isClosed_; 
	void close(); 
	// Event saving 
	pEventTable_t pEventTable_; 
	pNoteTable_t pNoteTable_; 
	EventMaskSet eventMaskSet_; 


	pH5ExperimentWeak_t parentExperiment_; 
	
	dispatchTable_t dispatchTable_; 
	namedSinkTable_t sourceToName_; 
	sourceSinkTable_t  sourceToSink_; 

	
	void dataSinkCacheFlushReload(); 


	H5::Group h5TSpikeGroup_; 
	bool haveTSpikeGroup_; 
	
	H5::Group h5WaveGroup_; 
	bool haveWaveGroup_; 


	bool isRecording_; 
	bool hasRecorded_; 

	Session currentSession_; 
	

	std::vector<Session> sessionCache_; 
	
	static void saveSessionAttribute(H5::Group node, const std::vector<Session>&); 
	static std::vector<Session> loadSessionAttribute(H5::Group node); 

// 	pNoteTable_t pNoteTable_; 
// 	pNoteTable_t openNoteTable(); 

	
	log4cpp::Category& logepoch_; 

      }; 

  }
}

#endif // H5EPOCH_H
