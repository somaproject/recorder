#ifndef H5FILEINTERFACE_H
#define H5FILEINTERFACE_H
#include <H5Cpp.h>
#include <hdf5.h>

#include "fileinterface.h" 

namespace soma {
  namespace recorder {
    class H5FileInterface : public FileInterface
    {
    public: 
      
      static pFileInterface_t openFile(filename_t name); // named constructor
      static pFileInterface_t createFile(filename_t name); 
      
      // implement the FileInterface
      pEpochInterface_t createEpoch(epochname_t name); 
      pEpochInterface_t getEpoch(epochname_t name); 
      std::list<EpochInterface> getAllEpochs(); 
      
      ~H5FileInterface(); 
      
    protected:
      H5FileInterface(H5::H5File fileid, filename_t name); 
      filename_t filename_; 
      H5::H5File h5file_; 
      
      
    }; 
  }
}

#endif // H5FILEINTERFACE_H
