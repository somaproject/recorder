
#include <boost/test/auto_unit_test.hpp>

#include "boost/filesystem/operations.hpp" // includes boost/filesystem/path.hpp
#include "boost/filesystem/fstream.hpp"    // ditto
#include <iostream>                        // for std::cout
#include "h5filerecorder.h"
using namespace soma; 
using  namespace boost;       
using namespace boost::filesystem; 

BOOST_AUTO_TEST_SUITE(H5FileRecorder); 

BOOST_AUTO_TEST_CASE(H5FileRecorder_test )
{
  
  std::string filename = "test.h5";
  filesystem::remove_all(filename); 
  recorder::H5FileRecorder * prec = new recorder::H5FileRecorder(filename); 

  prec->createEpoch("test1"); 
  prec->switchEpoch("test1"); 
  
  for (int i = 0; i < 1; i++ ) 
    {
      prec->enableRX(TSPIKE, i); 
      prec->enableRX(WAVE, i); 
    }

  delete prec; 
  
  // confirm existence of file
  path p(filename); 
  BOOST_CHECK( exists(p) );
  
  
}

BOOST_AUTO_TEST_SUITE_END(); 
