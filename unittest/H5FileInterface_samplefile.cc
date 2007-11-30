#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <iostream>                        
#include "h5fileinterface.h"
#include "epochinterface.h"
#include <somanetwork/tspike.h>
#include <somanetwork/wave.h>
#include <somanetwork/event.h>


using namespace soma; 
using namespace boost;       
using namespace boost::filesystem; 

BOOST_AUTO_TEST_SUITE(H5FileRecorderTestFile); 

void sleep1Data(recorder::pEpochInterface_t epoch)
{


  epoch->enableDataSink(0, TSPIKE, "hipp1"); 
  int N = 1000; 
  
  for (int i = 0; i < N; i++)
    {
      TSpike_t ts; 
      ts.src = 0; 
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
      epoch->appendData(rdp); 
      
    }



  epoch->enableDataSink(1, WAVE, "hipp2");   
  N = 1000; 
  
  uint64_t offsetval; 
  offsetval = 0x01234567; 
  offsetval = offsetval << 16; 
  offsetval |= 0x89AB; 
  for (uint64_t i = 0; i < N; i++)
    {
      Wave_t w; 
      w.src = 1; 
      w.time = i * offsetval;
      w.samprate = 0x789A; 
      w.selchan = 0x1234; 
      w.filterid = 0xAABB; 
      
      for (int j = 0; j < WAVEBUF_LEN; j++) {
	w.wave[j] = i * 0xABCD + j*0x17; 
      }
      
      pDataPacket_t rdp = rawFromWave(w); 
      epoch->appendData(rdp); 
    }

  
}




BOOST_AUTO_TEST_CASE(H5FileRecorderTestFile_sample)
{
  // Create a test file suitable for interpretation by people 
  // trying to reverse-engineer the format

  std::string filename = "soma.sample.h5"; 
  filesystem::remove_all(filename); 

  recorder::pFileInterface_t pFI =
    recorder::H5FileInterface::createFile(filename); 
  
  // -------------------------------------------------------
  // create SLEEP1 epoch
  // -------------------------------------------------------
  
  recorder::pEpochInterface_t sleep1 = pFI->createEpoch("sleep1"); 
  sleep1Data(sleep1); 

  
}




BOOST_AUTO_TEST_SUITE_END(); 
