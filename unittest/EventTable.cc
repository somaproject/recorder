
#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>                        
#include <fstream>
#include "eventtable.h"
using namespace soma; 
using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

BOOST_AUTO_TEST_SUITE(EventTableTest); 

BOOST_AUTO_TEST_CASE(EventTable_create)
{

  // create a temp file
  H5::H5File::H5File * h5file 
    = new H5::H5File::H5File("EventTable_create.h5", H5F_ACC_TRUNC); 

  H5::Group grp = h5file->createGroup("testGroup");
  
  recorder::pEventTable_t pET = recorder::EventTable::create(grp); 
  h5file->flush(H5F_SCOPE_GLOBAL); 

  h5file->close(); 
  delete h5file; 

  int retval = system("python EventTable_test.py create");
  BOOST_CHECK_EQUAL(retval , 0); 

}

BOOST_AUTO_TEST_CASE(EventTable_append)
{
  // create a temp file
  H5::H5File::H5File * h5file 
    = new H5::H5File::H5File("EventTable_append.h5", H5F_ACC_TRUNC); 
  
  H5::Group grp = h5file->createGroup("testGroup");
  
  {
  recorder::pEventTable_t  tst = recorder::EventTable::create(grp); 
  int N = 1000; 
  int emod = 17; 

  uint64_t offsetval; 
  for (int i = 0; i < N; i++)
    {
      
      recorder::SeqEvent_t se; 

      // now create N % 17 events
      for (int ne = 0; ne < (i % emod); ne++) {
	se.seq = i; 
	se.cmd = ne;
	se.src = (i + 100) % emod; 
	for (int d = 0; d < 5; d++) {
	  se.data[d] = d + i; 
	}
       tst->append(se); 
      }
    }
  }
  
  h5file->flush(H5F_SCOPE_GLOBAL); 
  
  h5file->close(); 
  
  delete h5file; 


  int retval = system("python EventTable_test.py append");
  BOOST_CHECK_EQUAL(retval , 0); 
  

}

BOOST_AUTO_TEST_CASE(EventTable_closeopen_Eventtest)
{
  // create a temp file
  
  {

  H5::H5File::H5File * h5file 
    = new H5::H5File::H5File("EventTable_closeopeneventtest.h5", H5F_ACC_TRUNC); 
  
  H5::Group grp = h5file->createGroup("testGroup");

  recorder::pEventTable_t  tst = recorder::EventTable::create(grp); 
  
  std::vector<eventcmd_t> cmds; 
  cmds.push_back(3); 
  cmds.push_back(17); 
  tst->addEventRXMask(0, cmds); 

  cmds.clear(); 
  cmds.push_back(23); 
  cmds.push_back(76); 
  tst->addEventRXMask(14, cmds); 

  h5file->flush(H5F_SCOPE_GLOBAL); 
  
  h5file->close(); 
  
  delete h5file; 
  }

  // now open
  H5::H5File hf; 
  hf.openFile("EventTable_closeopeneventtest.h5", H5F_ACC_RDWR); 
  H5::Group grp = hf.openGroup("testGroup");
  
  recorder::pEventTable_t  tst = recorder::EventTable::open(grp); 
  
  // now the checking
  std::vector<eventcmd_t> ts = tst->getEventRXMask(0); 
  BOOST_CHECK_EQUAL(ts.size(), 2); 
  BOOST_CHECK_EQUAL(ts[0], 3); 
  BOOST_CHECK_EQUAL(ts[1], 17); 
  
  ts = tst->getEventRXMask(14); 
  BOOST_CHECK_EQUAL(ts.size(), 2); 
  BOOST_CHECK_EQUAL(ts[0], 23); 
  BOOST_CHECK_EQUAL(ts[1], 76); 
  
  
}

BOOST_AUTO_TEST_SUITE_END(); 
