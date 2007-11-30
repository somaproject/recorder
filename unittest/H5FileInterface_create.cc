#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <iostream>                        
#include "fileinterface.h" 
#include "h5fileinterface.h"

using namespace soma; 
using namespace boost;       
using namespace boost::filesystem; 

BOOST_AUTO_TEST_SUITE(H5FileInterface); 

BOOST_AUTO_TEST_CASE(H5FileInterface_filecreate )
{
  // Test if file creation works; 
  
  std::string filename = "H5FileInterface_filecreate.h5"; 
  filesystem::remove_all(filename); 
  {
    // separate block to get delete called on object
    recorder::pFileInterface_t pFI =
      recorder::H5FileInterface::createFile(filename); 
    
  }
  // confirm existence of file
  path p(filename); 
  BOOST_CHECK( exists(p) );
  
}


BOOST_AUTO_TEST_CASE(H5FileInterface_filecreateexists )
{
  // Test if file creation fails when the file exists
  
  std::string filename = "H5FileInterface_filecreate_exist.h5"; 
  // create the file
  ofstream file(filename );
  file << "test output"; 
  file.close(); 
  
  BOOST_CHECK_THROW(recorder::H5FileInterface::createFile(filename), 
		    std::runtime_error); 
}

BOOST_AUTO_TEST_CASE(H5FileInterface_fileopen )
{
  // Test if file opening works
  
  std::string filename = "H5FileInterface_fileopen.h5"; 
  H5::H5File * h5f = new H5::H5File(filename,  H5F_ACC_TRUNC); 
  h5f->close(); 
  delete h5f; 

  path p(filename); 
  
  BOOST_CHECK( exists(p) ); // verify our create was successful. 
  // separate block to get delete called on object
  recorder::pFileInterface_t pFI =
    recorder::H5FileInterface::openFile(filename); 
  
}


BOOST_AUTO_TEST_CASE(H5FileInterface_fileopennotexists)
{
  // Test if file creation fails when the file exists
  
  std::string filename = "H5FileInterface_fileopen.h5"; 
  filesystem::remove_all(filename); 
  
  BOOST_CHECK_THROW(recorder::H5FileInterface::openFile(filename), 
		    std::runtime_error); 
}
		     
		     
BOOST_AUTO_TEST_SUITE_END(); 
