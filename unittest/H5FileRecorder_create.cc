#include <boost/test/auto_unit_test.hpp>

#include "boost/filesystem/operations.hpp" 
#include "boost/filesystem/fstream.hpp"    
#include <iostream>                        
#include "h5filerecorder.h"
using namespace soma; 
using  namespace boost;       
using namespace boost::filesystem; 

BOOST_AUTO_TEST_SUITE(H5FileRecorder); 

BOOST_AUTO_TEST_CASE(H5FileRecorder_filecreate )
{
  // Can we create the file? 

  std::string filename = "H5FileRecorder_filecreate.h5"; 
  filesystem::remove_all(filename); 
  recorder::H5FileRecorder * prec = new recorder::H5FileRecorder(filename); 

  delete prec; 
  
  // confirm existence of file
  path p(filename); 
  BOOST_CHECK( exists(p) );
  
  
}

BOOST_AUTO_TEST_CASE(H5FileRecorder_createepoch)
{
  std::string filename = "H5FileRecorder_createepoch.h5";
  filesystem::remove_all(filename); 
  recorder::H5FileRecorder * prec = new recorder::H5FileRecorder(filename); 

  prec->createEpoch("TestEpoch"); 
  prec->createEpoch("TestEpoch2"); 
  delete prec; 
  
  // confirm existence of file
  path p(filename); 
  BOOST_CHECK( exists(p) );
  
  int retval = system("python H5FileRecorder_create.py createepoch");
  BOOST_CHECK_EQUAL(retval , 0); 

}

BOOST_AUTO_TEST_CASE(H5FileRecorder_createTables)
{
  std::string filename = "H5FileRecorder_createTables.h5";
  filesystem::remove_all(filename); 
  recorder::H5FileRecorder * prec = new recorder::H5FileRecorder(filename); 

  prec->createEpoch("TestEpoch"); 
  prec->switchEpoch("TestEpoch"); 

  int N = 32; 
  for (int i = 0; i < N; i++ ) 
    {
      prec->enableDataRX(i, TSPIKE); 
      prec->enableDataRX(i, WAVE); 
    }
  
  delete prec; 
  
  // confirm existence of file
  path p(filename); 
  BOOST_CHECK( exists(p) );
  
  int retval = system("python H5FileRecorder_create.py createTables");
  BOOST_CHECK_EQUAL(retval , 0); 

}



BOOST_AUTO_TEST_SUITE_END(); 
