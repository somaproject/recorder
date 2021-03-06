
#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <iostream>                        
#include <fstream>

#include "tspiketable.h"
#include "test_config.h"
#include "test_util.h"

using namespace soma; 

using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

BOOST_AUTO_TEST_SUITE(TSpikeTableTest); 

BOOST_AUTO_TEST_CASE(TSpikeTable_create)
{
  // create a temp file
  path h5filepath = test_binary_path / "TSpikeTable_create.h5"; 

  H5::H5File::H5File * h5file 
    = new H5::H5File::H5File(h5filepath.string(), H5F_ACC_TRUNC); 
  H5::Group grp = h5file->createGroup("testGroup");
  
  int SRC = 17; 
  recorder::pTSpikeTable_t tst = recorder::TSpikeTable::create(grp,  "hipp1", SRC); 
  h5file->flush(H5F_SCOPE_GLOBAL); 
  h5file->close(); 
  delete h5file; 
  
  int retval = run_standard_py_script("TSpikeTable_test.py", 
				      "create"); 

  BOOST_CHECK_EQUAL(retval , 0); 
  
}

BOOST_AUTO_TEST_CASE(TSpikeTable_append)
{
  using namespace somanetwork; 

  // create a temp file
  path h5filepath = test_binary_path / "TSpikeTable_append.h5"; 

  H5::H5File::H5File * h5file =
    new H5::H5File::H5File(h5filepath.string(), H5F_ACC_TRUNC);

  H5::Group grp = h5file->createGroup("testGroup");
  
  int SRC = 23; 
  recorder::pTSpikeTable_t tst = recorder::TSpikeTable::create(grp, "hipp2", SRC); 
  int N = 1000; 
  
  
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
      
      pDataPacket_t rdp(rawFromTSpike(ts)); 
      tst->append(rdp); 
      
    }
  tst->flush(); 

  h5file->flush(H5F_SCOPE_GLOBAL); 
  h5file->close(); 
  delete h5file; 

  // verify the table
  

  int retval = run_standard_py_script("TSpikeTable_test.py", 
				      "append"); 

  BOOST_CHECK_EQUAL(retval , 0); 

}

BOOST_AUTO_TEST_SUITE_END(); 
