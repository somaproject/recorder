#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <iostream>   
#include <somanetwork/event.h>
#include "eventmaskset.h"

using namespace soma; 
using namespace soma::recorder; 
using namespace boost;       
using namespace boost::filesystem; 

BOOST_AUTO_TEST_SUITE(EventMaskSetTest) 

BOOST_AUTO_TEST_CASE(EventMaskSetCreate )
{
  // just create a mask set

  EventMaskSet es; 
  BOOST_CHECK_EQUAL(es.isSet(0, 0), false); 
  es.set(0, 0, true); 
  BOOST_CHECK_EQUAL(es.isSet(0, 0), true); 
  
  BOOST_CHECK_EQUAL(es.isSet(1, 10), false); 
  es.set(1, 10, true); 
  BOOST_CHECK_EQUAL(es.isSet(1, 10), true); 
  
}


BOOST_AUTO_TEST_CASE(EventMaskSetSetGroup )
{
  // just create a mask set

  EventMaskSet es; 
  eventsource_t SRC = 20; 
  std::vector<eventcmd_t> cmds; 
  cmds.push_back(7); 
  cmds.push_back(70); 
  cmds.push_back(200); 

  BOOST_CHECK_EQUAL(es.isSet(SRC, 7), false); 
  BOOST_CHECK_EQUAL(es.isSet(SRC, 70), false); 
  BOOST_CHECK_EQUAL(es.isSet(SRC, 200), false); 
  
  es.set(SRC, cmds, true); 

  BOOST_CHECK_EQUAL(es.isSet(SRC, 7), true); 
  BOOST_CHECK_EQUAL(es.isSet(SRC, 70), true); 
  BOOST_CHECK_EQUAL(es.isSet(SRC, 200), true); 

  es.set(SRC, cmds, false); 
  
  BOOST_CHECK_EQUAL(es.isSet(SRC, 7), false); 
  BOOST_CHECK_EQUAL(es.isSet(SRC, 70), false); 
  BOOST_CHECK_EQUAL(es.isSet(SRC, 200), false); 
  
  
}

BOOST_AUTO_TEST_SUITE_END(); 
