#ifndef H5FILERECORDER_H
#define H5FILERECORDER_H


#include <string>
#include <H5Cpp.h>
#include <map>
#include <list>
#include "datasetio.h"

typedef std::string epochname_t; 
namespace soma 
{ 
  namespace recorder {
    typedef std::pair< datasource_t, datatype_t> dpair_t; 

    class H5FileRecorder
      { 

      public: 
	// recorder construction

	H5FileRecorder(const std::string & filename); 
	~H5FileRecorder(); 
	
	// epoch manipulation
	void createEpoch(const epochname_t & epochName); 
	std::list<epochname_t> getAvailableEpochs(); 
	
	void enableDataRX(std::string name, datasource_t src, 
			  datatype_t typ); 
	
	void disableDataRX(datasource_t src, datatype_t typ); 
	void disableDataRX(std::string name); 


	// Recording settings
	void startRecording(epochname_t); 
	void pauseRecording(); 
	void stopRecording(); 
	 
	// take in data, etc. 
	void appendData(pDataPacket_t rdp); 
	void appendEvent(pEventPacket_t ep); 
	void appendString(std::string_t); 

      private:
	std::string filename_; 
	H5::H5File h5file_; 
	H5::Group recordingEpochGroup_;
	std::map<epochname_t, H5::Group> epochs_; 

	H5::Group getTypeGroup(datatype_t typ);

	dispatchTable_t dispatchTable_; 
	  friend void H5FileRecorder_test(); 
	  
      };
    
  }
}

#endif // H5FILERECORDER_H
