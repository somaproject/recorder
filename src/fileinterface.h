#ifndef FILEINTERFACE_H
#define FILEINTERFACE_H

#include <string>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include "epochinterface.h"
#include "notes.h"

namespace  soma { 
  namespace recorder { 
    
    typedef std::string filename_t; 
    
    class FileInterface : boost::noncopyable
      {
	// abstract base class for persistent storage. 
      public: 
	
	virtual pEpochInterface_t createEpoch(epochname_t name) = 0; 
	virtual pEpochInterface_t getEpoch(epochname_t name) = 0; 
	virtual std::list<EpochInterface> getAllEpochs() = 0; 
	
      }; 
    
    typedef boost::shared_ptr<FileInterface> pFileInterface_t; 
  }
}

#endif // FILEINTERFACE_H 
