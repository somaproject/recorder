#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <iostream>                        
#include <H5Cpp.h>

#include "epochinterface.h"
#include "h5epoch.h"

using namespace soma;
using namespace soma::recorder; 
using namespace boost;       
using namespace boost::filesystem; 

BOOST_AUTO_TEST_SUITE(H5Epoch_test); 

struct dataSinkTestSet
{
  datasource_t src; 
  datatype_t typ; 
  std::string name; 
  bool deleted; 
}; 

dataSinkTestSet dataSinkTestSetCreate (datasource_t src, datatype_t typ, 
				       std::string name,
				       bool deleted){

  dataSinkTestSet d1; 
  d1.src = src; 
  d1.typ = typ; 
  d1.name = name; 
  d1.deleted = deleted; 
  return d1; 
}

std::list<dataSinkTestSet> generateDataSinkSets()
{
  /// a generic set of data sinks to work with later
  std::list<dataSinkTestSet> sets; 
  sets.push_back(dataSinkTestSetCreate(0,  TSPIKE, "hipp1",    false)); 
  sets.push_back(dataSinkTestSetCreate(1,  TSPIKE, "hipp2",    false)); 
  sets.push_back(dataSinkTestSetCreate(5,  TSPIKE, "hipp3",    true)); 
  sets.push_back(dataSinkTestSetCreate(12, TSPIKE, "hipp4",    false)); 
  sets.push_back(dataSinkTestSetCreate(0,  WAVE,   "hipcont1", false)); 
  sets.push_back(dataSinkTestSetCreate(20, WAVE,   "hipcont2", false)); 
  sets.push_back(dataSinkTestSetCreate(7,  WAVE, "ec1", true)); 
  sets.push_back(dataSinkTestSetCreate(9,  WAVE, "ec2", true)); 
  sets.push_back(dataSinkTestSetCreate(10, WAVE, "ec3", false)); 
  return sets; 

}

BOOST_AUTO_TEST_CASE(H5Epoch_create)
{
  // can we create a simple epoch
  std::string filename("H5Epoch_test_create.h5"); 

  H5::H5File h5File(filename, H5F_ACC_TRUNC); 
  std::string epochName = "myEpoch"; 

  H5::Group group =  h5File.createGroup(epochName);
  
  
  recorder::pEpochInterface_t ei(new recorder::H5Epoch(group, epochName)); 
  
  BOOST_CHECK_EQUAL(ei->getName(), epochName); 
  
}


BOOST_AUTO_TEST_CASE(H5Epoch_datasink_enableerrorcheck)
{
  // enable a single data sink and then try and reenable it and 
  // make sure we generate exceptions correctly


  std::string filename("H5Epoch_datasink_enablecheck.h5"); 

  H5::H5File h5File(filename, H5F_ACC_TRUNC); 
  std::string epochName = "myEpoch"; 

  H5::Group group =  h5File.createGroup(epochName);
  
  recorder::pEpochInterface_t ei(new recorder::H5Epoch(group, epochName)); 
  
  ei->enableDataSink(17, TSPIKE, "testsink"); 

  // try and enable same thing
  BOOST_CHECK_THROW(ei->enableDataSink(17, TSPIKE, "testsink"), 
		    std::runtime_error); 

  // try with an already-used name
  BOOST_CHECK_THROW(ei->enableDataSink(18, TSPIKE, "testsink"), 
		    std::runtime_error); 

  ei->enableDataSink(17, WAVE, "testsink");  // should be okay, different type
  

}


BOOST_AUTO_TEST_CASE(H5Epoch_datasink)
{
  // we enable and disable a bunch of data sources and then 
  // check to see that we've enabled / disabled the correct ones

  // CREATE FILE

  std::string filename("H5Epoch_test_create.h5"); 

  H5::H5File h5File(filename, H5F_ACC_TRUNC); 
  std::string epochName = "myEpoch"; 

  H5::Group group =  h5File.createGroup(epochName);
  
  recorder::pEpochInterface_t ei(new recorder::H5Epoch(group, epochName)); 
  
  std::list<dataSinkTestSet> sets = generateDataSinkSets(); 

  // create 
  for (std::list<dataSinkTestSet>::iterator s = sets.begin(); 
       s != sets.end(); ++s)
    {
      ei->enableDataSink(s->src, s->typ, s->name); 
    }


  // disable the ones we've set deleted to true for
  for (std::list<dataSinkTestSet>::iterator s = sets.begin(); 
       s != sets.end(); ++s)
    {
      if (s->deleted) {
	ei->disableDataSink(s->src, s->typ); 
      }
    }
  
  // now verify via name and properties
    for (std::list<dataSinkTestSet>::iterator s = sets.begin(); 
       s != sets.end(); ++s)
    {
      // is the name correct? 
      if (not s->deleted) {
	BOOST_CHECK_EQUAL(ei->getDataSinkName(s->src, s->typ), s->name); 
      }
      
    }
    
}

BOOST_AUTO_TEST_CASE(H5Epoch_datasink_getall)
{
  // we enable and disable a bunch of data sources and then 
  // check to see if we can get them all 

  // CREATE FILE

  std::string filename("H5Epoch_test_datasinkall.h5"); 

  H5::H5File h5File(filename, H5F_ACC_TRUNC); 
  std::string epochName = "myEpoch"; 

  H5::Group group =  h5File.createGroup(epochName);
  
  recorder::pEpochInterface_t ei(new recorder::H5Epoch(group, epochName)); 
  
  std::list<dataSinkTestSet> sets = generateDataSinkSets(); 

  // create 
  for (std::list<dataSinkTestSet>::iterator s = sets.begin(); 
       s != sets.end(); ++s)
    {
      ei->enableDataSink( s->src, s->typ, s->name); 
    }


  std::list<dpair_t> sinks = ei->getAllDataSinks(); 
  
  
  // disable the ones we've set deleted to true for
  for (std::list<dataSinkTestSet>::iterator s = sets.begin(); 
       s != sets.end(); ++s)
    {
      dpair_t d(s->src, s->typ); 
      std::list<dpair_t>::iterator iter = std::find(sinks.begin(), 
						    sinks.end(), d); 

      BOOST_CHECK(iter != sinks.end()); 
    }
  
}


BOOST_AUTO_TEST_SUITE_END(); 
