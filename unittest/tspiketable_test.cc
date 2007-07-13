
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
  H5::H5File::H5File h5file("tspikecreate.h5", H5F_ACC_TRUNC); 
  H5::Group grp = h5file.createGroup("testGroup");
  
  int SRC = 17; 
  TSpikeTable tst(17, grp); 
  h5file.close(); 

}

BOOST_AUTO_TEST_CASE(TSpikeTable_append)
{
  // create a temp file
  H5::H5File::H5File h5file("tspikeappend.h5", H5F_ACC_TRUNC);

  H5::Group grp = h5file.createGroup("testGroup");
  
  int SRC = 23; 
  TSpikeTable tst(23, grp); 
  int N = 2; 
  
  
  for (int i = 0; i < N; i++)
    {
      TSpike_t ts; 
      ts.src = SRC; 
      ts.time = i; 
      TSpikeWave_t * tswp[4]; 
      tswp[0] = &ts.x; 
      tswp[1] = &ts.y; 
      tswp[2] = &ts.a; 
      tswp[3] = &ts.b; 
      
      for (int j = 0; j < 4; j++) {
	TSpikeWave_t * tw = tswp[j]; 
	tw->filtid = i * 5 + j; 
	tw->valid = 1; 
	tw->threshold = i * 0x1234 + j * 0xAB; 
	for (int k = 0; k < TSPIKEWAVE_LEN; k++) {
	  tw->wave[k] = i + j + k; 
	}
      }
      
      DataPacket_t * rdp = rawFromTSpike(ts); 
      tst.append(rdp); 
      delete rdp; 
    }

  h5file.close(); 

  // verify the table
  H5::H5File::H5File h5fileRead("tspikeappend.h5", H5F_ACC_RDONLY); 
  H5::Group grpread = h5fileRead.openGroup("testGroup"); 
  


}

BOOST_AUTO_TEST_SUITE_END(); 
