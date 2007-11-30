
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

BOOST_AUTO_TEST_SUITE(WaveTableTest); 

BOOST_AUTO_TEST_CASE(WaveTable_create)
{

  // create a temp file
  H5::H5File::H5File * h5file 
    = new H5::H5File::H5File("WaveTable_create.h5", H5F_ACC_TRUNC); 

  H5::Group grp = h5file->createGroup("testGroup");
  
  int SRC = 17; 
  WaveTable tst(SRC, "eeg1",  grp); 
  h5file->flush(H5F_SCOPE_GLOBAL); 

  h5file->close(); 
  delete h5file; 

  int retval = system("python WaveTable_test.py create");
  BOOST_CHECK_EQUAL(retval , 0); 

}

BOOST_AUTO_TEST_CASE(WaveTable_append)
{
  // create a temp file
  H5::H5File::H5File * h5file 
    = new H5::H5File::H5File("WaveTable_append.h5", H5F_ACC_TRUNC); 

  H5::Group grp = h5file->createGroup("testGroup");
  
  int SRC = 23; 
  WaveTable*  tst = new WaveTable(23, "eeg2", grp); 
  int N = 1000; 
  
  uint64_t offsetval; 
  offsetval = 0x01234567; 
  offsetval = offsetval << 16; 
  offsetval |= 0x89AB; 
  for (uint64_t i = 0; i < N; i++)
    {
      Wave_t w; 
      w.src = SRC; 
      w.time = i * offsetval;
      w.samprate = 0x789A; 
      w.selchan = 0x1234; 
      w.filterid = 0xAABB; 
      
      for (int j = 0; j < WAVEBUF_LEN; j++) {
	w.wave[j] = i * 0xABCD + j*0x17; 
      }
      
      pDataPacket_t rdp = rawFromWave(w); 
      tst->append(rdp); 
      rdp.reset(); 


    }
  delete tst; 
  h5file->flush(H5F_SCOPE_GLOBAL); 

  h5file->close(); 
  h5file->flush(H5F_SCOPE_GLOBAL); 

  delete h5file; 


  int retval = system("python WaveTable_test.py append");
  BOOST_CHECK_EQUAL(retval , 0); 
  

}

BOOST_AUTO_TEST_SUITE_END(); 
