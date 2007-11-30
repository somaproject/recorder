
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
  
  recorder::EventTable tst(grp); 
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
  
  recorder::EventTable*  tst = new recorder::EventTable(grp); 
  int N = 1000; 
  int emod = 17; 

  uint64_t offsetval; 
  for (int i = 0; i < N; i++)
    {
      pEventPacket_t pep(new EventPacket_t); 
      pep->events.reset(new EventList_t()); 

      pep->seq = i; 
      pep->missing = false; 
      
      // now create N % 17 events
      for (int ne = 0; ne < (i % emod); ne++) {

	Event_t e; 
	e.cmd = ne;
	e.src = (i + 100) % emod; 
	for (int d = 0; d < 5; d++) {
	  e.data[d] = d + i; 
	}
	pep->events->push_back(e); 
      }
      
      
      tst->append(pep); 
      
    }
  delete tst; 
  h5file->flush(H5F_SCOPE_GLOBAL); 
  
  h5file->close(); 
  
  delete h5file; 


  int retval = system("python EventTable_test.py append");
  BOOST_CHECK_EQUAL(retval , 0); 
  

}

BOOST_AUTO_TEST_SUITE_END(); 
