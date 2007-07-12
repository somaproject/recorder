
#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>                        
#include <fstream>
#include "h5filerecorder.h"
#include "wavetable.h"
using namespace soma; 
using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

BOOST_AUTO_TEST_SUITE(WaveTable); 

BOOST_AUTO_TEST_CASE(WaveTable_create)
{
  // create a temp file
  H5::H5File::H5File h5file("wavecreate.h5", H5F_ACC_TRUNC); 
  H5::Group grp = h5file.createGroup("testGroup");
  
  int SRC = 17; 
  WaveTable tst(17, grp); 
  h5file.close(); 

}

BOOST_AUTO_TEST_CASE(WaveTable_append)
{
  // create a temp file
  H5::H5File::H5File h5file("waveappend.h5", H5F_ACC_TRUNC); 
  H5::Group grp = h5file.createGroup("testGroup");
  
  int SRC = 23; 
  WaveTable tst(23, grp); 
  int N = 100; 
  
  
  for (int i = 0; i < N; i++)
    {
      Wave_t w; 
      w.src = SRC; 
      w.time = i * 0x0123456789ABC; 
      w.samprate = 0x789A; 
      w.selchan = 0x1234; 
      w.filterid = 0xAABB; 
      
      for (int j = 0; j < WAVEBUF_LEN; j++) {
	w.wave[j] = i * 0xABCD + j*0x17; 
      }
      
      DataPacket_t * rdp = rawFromWave(w); 
      tst.append(rdp); 
      delete rdp; 

    }

  h5file.close(); 

  

}

BOOST_AUTO_TEST_SUITE_END(); 
