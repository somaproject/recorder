#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <iostream>   
#include <somanetwork/fakenetwork.h>
#include <set>
#include "experiment.h"                     
#include "epoch.h"                     
#include "h5epoch.h"                     
#include "h5experiment.h"

#include "datasetio.h"
#include "test_config.h"
#include "test_util.h"

using namespace soma; 
using namespace boost;       
using namespace boost::filesystem; 

BOOST_AUTO_TEST_SUITE(H5Epoch); 

BOOST_AUTO_TEST_CASE(H5EpochCreate )
{
  // Test if file creation works; 
  
  std::string filename = "H5Epoch_create.h5"; 
  path h5filepath = test_binary_path /  filename; 
  
  filesystem::remove_all(h5filepath); 

  // separate block to get delete called on object
  pNetworkInterface_t pfn(new FakeNetwork()); 

  recorder::pExperiment_t pExp =
    recorder::H5Experiment::create(pfn, filename); 
  
  recorder::pEpoch_t epoch  = pExp->createEpoch("Silly"); 
  
  BOOST_CHECK_EQUAL(epoch->getName(), "Silly"); 

  BOOST_CHECK_THROW(pExp->createEpoch("Silly"), 
		    std::runtime_error); 
  
}



BOOST_AUTO_TEST_CASE(H5EpochDataEnable)
{
  /*
    If we enable data sources, do we create the correct groups? 
    
    We create a file and then check if we have proper children. 

  */
  
  std::string filename = "H5Epoch_dataenable.h5"; 
  path h5filepath = test_binary_path /  filename; 
  filesystem::remove_all(h5filepath); 

  // separate block to get delete called on object
  pNetworkInterface_t pfn(new FakeNetwork()); 

  recorder::pExperiment_t pExp =
    recorder::H5Experiment::create(pfn, h5filepath); 
  
  recorder::pEpoch_t epoch  = pExp->createEpoch("Silly"); 
  
  epoch->enableDataSink(0, TSPIKE); 
  epoch->enableDataSink(7, TSPIKE); 
  epoch->enableDataSink(9, TSPIKE); 
  epoch->enableDataSink(3, TSPIKE); 
  epoch->disableDataSink(9, TSPIKE); 
  epoch->disableDataSink(3, TSPIKE); 
  pExp->close(); 
    
  // Now read the file, check
  H5::H5File newfile; 
  newfile.openFile(h5filepath.string(), H5F_ACC_RDWR); 
  H5::Group g = newfile.openGroup("/Silly/TSpike"); 
  BOOST_CHECK_EQUAL(g.getNumObjs(), 2); 
  
}

BOOST_AUTO_TEST_CASE(H5EpochDatagetSets)
{
  /*
    If we enable data sources, check if we get the correct sets out

  */
  
  std::string filename = "H5Epoch_datatypesets.h5"; 
  path h5filepath = test_binary_path /  filename; 

  filesystem::remove_all(h5filepath); 

  // separate block to get delete called on object
  pNetworkInterface_t pfn(new FakeNetwork()); 

  recorder::pExperiment_t pExp =
    recorder::H5Experiment::create(pfn, h5filepath); 
  
  recorder::pEpoch_t epoch  = pExp->createEpoch("Silly"); 
  
  epoch->enableDataSink(0, TSPIKE); 
  epoch->enableDataSink(7, TSPIKE); 
  epoch->enableDataSink(9, TSPIKE); 
  epoch->disableDataSink(7, TSPIKE); 

  std::set<recorder::dpair_t> sets = epoch->getDataSinks(); 
  BOOST_CHECK_EQUAL(sets.size(), 2); 
  
}


BOOST_AUTO_TEST_CASE(H5Epoch_rename)
{
  /*
    Can we rename sources

  */
  
  std::string filename = "H5Epoch_rename.h5"; 
  path h5filepath = test_binary_path /  filename; 
 filesystem::remove_all(h5filepath); 

  // separate block to get delete called on object
  pNetworkInterface_t pfn(new FakeNetwork()); 

  recorder::pExperiment_t pExp =
    recorder::H5Experiment::create(pfn, h5filepath); 
  
  recorder::pEpoch_t epoch  = pExp->createEpoch("Silly"); 
  
  epoch->enableDataSink(0, TSPIKE); 
  epoch->setDataName(0, "HelloWorld0"); 
  BOOST_CHECK_EQUAL(epoch->getDataName(0), "HelloWorld0"); 

  epoch->enableDataSink(7, TSPIKE); 
  epoch->enableDataSink(7, WAVE); 
  epoch->setDataName(7, "HelloWorld7"); 
  epoch->setDataName(7, "GoodbyeWorld7"); 
  BOOST_CHECK_EQUAL(epoch->getDataName(7), "GoodbyeWorld7"); 

  pExp->close(); 
  
  // now try and read it! 
  int retval = run_standard_py_script("H5Epoch_test.py", 
				      "rename"); 
  BOOST_CHECK_EQUAL(retval , 0); 

}
		     
BOOST_AUTO_TEST_SUITE_END(); 
