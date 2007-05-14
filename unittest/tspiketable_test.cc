
#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>                        
#include "h5filerecorder.h"
#include "tspiketable.h"
using namespace soma; 
using  namespace boost;       
using namespace boost::filesystem; 

BOOST_AUTO_TEST_SUITE(TSpikeTable); 

BOOST_AUTO_TEST_CASE(TSpikeTable_create)
{
  // create a temp file
  H5::H5File::H5File h5file("create.h5", H5F_ACC_TRUNC); 
  H5::Group grp = h5file.createGroup("testGroup");
  
  int SRC = 17; 
  TSpikeTable tst(17, grp); 
  h5file.close(); 

}

BOOST_AUTO_TEST_CASE(TSpikeTable_append)
{
  // create a temp file
  H5::H5File::H5File h5file("append.h5", H5F_ACC_TRUNC); 
  H5::Group grp = h5file.createGroup("testGroup");
  
  int SRC = 23; 
  TSpikeTable tst(23, grp); 
  int N = 1000; 
  for (int i = 0; i < N; i++)
    {
      TSpike_t t; 
      t.src = 17; 
      t.time = i; 

      RawData * x = rawFromTSpike(t); 
      tst.append(x); 
      delete x; 

    }



  h5file.close(); 

}

BOOST_AUTO_TEST_SUITE_END(); 
