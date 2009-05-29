
#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>                        
#include <fstream>
#include "wavetable.h"
#include "test_config.h"
#include "test_util.h"

using namespace soma; 
using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

BOOST_AUTO_TEST_SUITE(WaveTableTest); 

BOOST_AUTO_TEST_CASE(WaveTable_create)
{
  using namespace somanetwork; 
  path h5filepath = test_binary_path / "WaveTable_create.h5"; 

  // create a temp file
  H5::H5File::H5File * h5file 
    = new H5::H5File::H5File(h5filepath.string(), H5F_ACC_TRUNC); 

  H5::Group grp = h5file->createGroup("testGroup");
  
  int SRC = 17; 
  recorder::pWaveTable_t  tst = recorder::WaveTable::create(grp, "eeg1", SRC); 
  h5file->flush(H5F_SCOPE_GLOBAL); 

  h5file->close(); 
  delete h5file; 
  int retval = run_standard_py_script("WaveTable_test.py", 
				      "create"); 
  BOOST_CHECK_EQUAL(retval , 0); 

}

BOOST_AUTO_TEST_CASE(WaveTable_append)
{
  using namespace somanetwork; 
  // create a temp file
  path h5filepath = test_binary_path / "WaveTable_append.h5"; 

  H5::H5File::H5File * h5file 
    = new H5::H5File::H5File(h5filepath.string(), H5F_ACC_TRUNC); 

  H5::Group grp = h5file->createGroup("testGroup");
  
  int SRC = 23; 
  recorder::pWaveTable_t  tst = recorder::WaveTable::create(grp, "eeg2", SRC); 
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
      w.sampratenum = 0x789A; 
      w.samprateden = 0x1122; 
      w.selchan = 0x1234; 
      w.filterid = 0xAABBCCD; 
      
      for (int j = 0; j < WAVEBUF_LEN; j++) {
	w.wave[j] = i * 0xABCD + j*0x17; 
      }
      
      pDataPacket_t rdp = rawFromWave(w); 
      tst->append(rdp); 
      rdp.reset(); 


    }
   tst->flush(); 
  h5file->flush(H5F_SCOPE_GLOBAL); 

  h5file->close(); 

  delete h5file; 


  int retval = run_standard_py_script("WaveTable_test.py", 
				      "append"); 
  BOOST_CHECK_EQUAL(retval , 0); 

}

BOOST_AUTO_TEST_SUITE_END(); 
