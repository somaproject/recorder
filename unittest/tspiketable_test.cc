
#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>                        
#include <fstream>
#include "h5filerecorder.h"
#include "tspiketable.h"
using namespace soma; 
using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

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
  int N = 100; 

  std::fstream pyfile("frompy.dat", ios::in | ios::binary); 
      
  for (int i = 0; i < N; i++)
    {
      RawData * rdp = new RawData(); 
      const int PACKLEN = 548; 
      char buffer[PACKLEN]; 
      
      pyfile.read(buffer, PACKLEN); 
      memcpy(&rdp->body[0], buffer, PACKLEN); 
      
      tst.append(rdp); 
      delete rdp; 

    }



  h5file.close(); 

}

BOOST_AUTO_TEST_SUITE_END(); 
