
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <H5Cpp.h>
#include "h5fileinterface.h"
#include "h5epoch.h"

using namespace soma::recorder; 
pFileInterface_t H5FileInterface::openFile(filename_t name) {
  // named constructor to open an existing file
  
  // check for existence
  boost::filesystem::path p(name); 

  if (not boost::filesystem::exists(p) )
    {
      boost::format errormsg("File %s does not exist." ); 
      throw std::runtime_error(boost::str(errormsg % name) ) ; 
    }

  // try and open the file
  H5::H5File file; 
  file.openFile(name, H5F_ACC_RDWR); 

  // pass to the real constructor
  return pFileInterface_t(new H5FileInterface(file, name)); 
  
}

pFileInterface_t H5FileInterface::createFile(filename_t name) {
  // named constructor to create a new file
  boost::filesystem::path p(name); 

  if (boost::filesystem::exists(p) )
    {
      boost::format errormsg("File %s already exists." ); 
      throw std::runtime_error(boost::str(errormsg % name) ) ; 
    }
  

  H5::H5File file(name, H5F_ACC_TRUNC); 
  
  // pass to the real constructor
  return pFileInterface_t(new H5FileInterface(file, name)); 

}

H5FileInterface::H5FileInterface(H5::H5File fileid, filename_t name) :
  h5file_(fileid), 
  filename_(name)
{



}

H5FileInterface::~H5FileInterface() {



}

pEpochInterface_t H5FileInterface::createEpoch(epochname_t name) 
{
  
  pEpochInterface_t pe(new H5Epoch(h5file_.openGroup("/"), name)); 
  return pe; 

}

pEpochInterface_t H5FileInterface::getEpoch(epochname_t name) 
{


}

std::list<pEpochInterface_t> H5FileInterface::getAllEpochs() 
{


}

