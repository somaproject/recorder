#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <iostream>                        
#include <H5Cpp.h>
#include <tspiketable.h>
#include "wavetable.h"

#include "epochinterface.h"
#include "h5epoch.h"

using namespace soma; 
using namespace boost;       
using namespace boost::filesystem; 

BOOST_AUTO_TEST_SUITE(H5Epoch_appenddata); 

BOOST_AUTO_TEST_CASE(H5Epoch_appenddata1)
{
  // we enable and disable a bunch of data sources and then 
  // check to see if we can get them all 

  // CREATE FILE
  {
  std::string filename("H5Epoch_appenddata1.h5"); 

  H5::H5File h5File(filename, H5F_ACC_TRUNC); 
  std::string epochName = "myEpoch"; 

  H5::Group group =  h5File.createGroup(epochName);
  
  recorder::pEpochInterface_t ei(new recorder::H5Epoch(group, epochName)); 

  int TSRC = 23;   
  int WSRC = 7; 
  ei->enableDataSink(TSRC, TSPIKE, "hipp1"); 
  ei->enableDataSink(WSRC, WAVE, "hipp4"); 

  // ADD THE DATA

  int N = 100; 
  
  for (int i = 0; i < N; i++)
    {
      TSpike_t ts; 
      ts.src = TSRC; 
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
      
      pDataPacket_t rdp(rawFromTSpike(ts)); 
      ei->appendData(rdp); 
    }
  
  N = 200; 
  uint64_t offsetval; 
  offsetval = 0x01234567; 
  offsetval = offsetval << 16; 
  offsetval |= 0x89AB; 
  for (uint64_t i = 0; i < N; i++)
    {
      Wave_t w; 
      w.src = WSRC; 
      w.time = i * offsetval;
      w.samprate = 0x789A; 
      w.selchan = 0x1234; 
      w.filterid = 0xAABB; 
      
      for (int j = 0; j < WAVEBUF_LEN; j++) {
	w.wave[j] = i * 0xABCD + j*0x17; 
      }
      
      pDataPacket_t rdp = rawFromWave(w); 
      ei->appendData(rdp); 


    }
  // now we need to flush the file
  }
  int retval = system("python H5Epoch_appenddata.py appenddata1");
  BOOST_CHECK_EQUAL(retval , 0); 

}


BOOST_AUTO_TEST_SUITE_END(); 
