#ifndef H5FILERECORDER_H
#define H5FILERECORDER_H


#include <string>
#include <H5Cpp.h>
#include <map>
#include <list>
#include "datasetio.h"

namespace soma 
{ 
  namespace recorder {
    typedef std::pair< datasource_t, datatype_t> dpair_t; 
    typedef std::map< dpair_t, DatasetIO *> dispatchTable_t; 
    class H5FileRecorder
      { 
      public: 
	H5FileRecorder(const std::string & filename); 
	~H5FileRecorder(); 
	void createEpoch(const std::string & epochName); 
	void switchEpoch(const std::string & epochName); 
	void enableRX(datasource_t src, datatype_t typ); 
	void disableRX(datasource_t src, datatype_t typ); 

	std::list<dpair_t> getDataRX(); 
	void append(DataPacket_t * rdp); 
      private:
	std::string filename_; 
	H5::H5File h5file_; 
	H5::Group epochGroup_;

	H5::Group getTypeGroup(datatype_t typ);
	dispatchTable_t dispatchTable_; 
	  friend void H5FileRecorder_test(); 
	
      };
    
  }
}

#endif // H5FILERECORDER_H
