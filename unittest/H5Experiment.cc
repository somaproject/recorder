#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <iostream>   

#include <fstream>

#include <boost/shared_ptr.hpp>
#include <somanetwork/fakenetwork.h>
#include "experiment.h"  
#include "epoch.h"                   
#include "h5experiment.h"

using namespace soma; 
using namespace boost;       
using namespace boost::filesystem; 

BOOST_AUTO_TEST_SUITE(H5Experiment); 

BOOST_AUTO_TEST_CASE(H5Experiment_filecreate )
{
  // Test if file creation works; 
  
  std::string filename = "H5Experiment_filecreate.h5"; 
  filesystem::remove_all(filename); 
  {
    // separate block to get delete called on object
    pNetworkInterface_t pfn(new FakeNetwork()); 
    recorder::H5Experiment::create(pfn, filename); 

  }
  // confirm existence of file
  path p(filename); 
  BOOST_CHECK( exists(p) );
  
}


BOOST_AUTO_TEST_CASE(H5FileInterface_filecreateexists )
{
  // Test if file creation fails when the file exists
  
  std::string filename = "H5Experiment_filecreate_exist.h5"; 
  // create the file
  std::ofstream file(filename.c_str() );
  file << "test output"; 
  file.close(); 
  pNetworkInterface_t pfn(new FakeNetwork()); 
    
  BOOST_CHECK_THROW(recorder::H5Experiment::create(pfn, filename), 
		    std::runtime_error); 
}

BOOST_AUTO_TEST_CASE(H5FileInterface_fileopen )
{
  // Test if file opening works
  
  std::string filename = "H5FileInterface_fileopen.h5"; 
  H5::H5File * h5f = new H5::H5File(filename,  H5F_ACC_TRUNC); 
  h5f->close(); 
  delete h5f; 

  path p(filename); 
  
  BOOST_CHECK( exists(p) ); // verify our create was successful. 
  // separate block to get delete called on object

  pNetworkInterface_t pfn(new FakeNetwork()); 

  recorder::pExperiment_t pFI =
    recorder::H5Experiment::open(pfn, filename); 
  
  BOOST_CHECK_THROW(recorder::H5Experiment::open(pfn, filename + "ERROR"), 
		    std::runtime_error)
  
}


BOOST_AUTO_TEST_CASE(H5FileInterface_fileopennotexists)
{
  // Test if file creation fails when the file exists
  
  std::string filename = "H5FileInterface_fileopen.h5"; 
  filesystem::remove_all(filename); 
  
  pNetworkInterface_t pfn(new FakeNetwork()); 

  BOOST_CHECK_THROW(recorder::H5Experiment::open(pfn, filename), 
		    std::runtime_error); 
}
		     


BOOST_AUTO_TEST_SUITE_END(); 



BOOST_AUTO_TEST_SUITE(H5Experiment_epochmanipulation); 

BOOST_AUTO_TEST_CASE(H5Experiment_epochnums )
{
  /*
    1. can we create multiple epochs
    2. do we get them back when we call getEpochs()
    3. can we get them individually via getEpoch(name)?
  */
  
  std::string filename = "H5Experiment_epochmanip.h5"; 
  filesystem::remove_all(filename); 

  // separate block to get delete called on object
  pNetworkInterface_t pfn(new FakeNetwork()); 

  recorder::pExperiment_t pExp =
    recorder::H5Experiment::create(pfn, filename); 
  
  typedef std::vector<std::string > svect_t;
  typedef std::vector<recorder::pEpoch_t> evect_t; 

  svect_t epochnames; 
  evect_t epochs; 

  epochnames.push_back("Sam"); 
  epochnames.push_back("Jack"); 
  epochnames.push_back("Daniel"); 
  epochnames.push_back("Tealc"); 
  
  
  for (svect_t::iterator name = epochnames.begin(); 
       name != epochnames.end(); name++) {
    
    recorder::pEpoch_t epoch  = pExp->createEpoch(*name); 
    epochs.push_back(epoch); 
    BOOST_CHECK_EQUAL(epoch->getName(), *name); 
    
  }
  
  // did we get the right number? 
  BOOST_CHECK_EQUAL(pExp->getEpochs().size(), epochnames.size()); 
  
  
  // Now check if we have returned them in the right order:
  for (int i = 0; i < epochnames.size(); i++) {
    BOOST_CHECK_EQUAL(pExp->getEpochs()[i], epochs[i]);
    BOOST_CHECK_EQUAL(pExp->getEpochs()[i]->getName(), epochnames[i]);
  }

  for (int i = 0; i < epochnames.size(); i++) {
    recorder::pEpoch_t epoch = pExp->getEpoch(epochnames[i]); 
    pExp->deleteEpoch(epoch); 
  }

  BOOST_CHECK_EQUAL(pExp->getEpochs().size(), 0); 
  
  // save the file
  pExp->close(); 
  
  // now try and reopen it
  H5::H5File newfile; 
  newfile.openFile(filename, H5F_ACC_RDWR); 
  // now count the number of children -- should just be notes, i.e. 1
  BOOST_CHECK_EQUAL(newfile.getNumObjs(), 1); 

}

BOOST_AUTO_TEST_CASE(H5Experiment_epochload )
{
  /*
    1. create multiple epochs
    2. close and reopen file
    3. check we have at least superficially recovered the epochs
  */
  
  std::string filename = "H5Experiment_epochreload.h5"; 
  filesystem::remove_all(filename); 
    
  typedef std::vector<std::string > svect_t;
  
  svect_t epochnames; 
  
  epochnames.push_back("Sam"); 
  epochnames.push_back("Jack"); 
  epochnames.push_back("Daniel"); 
  epochnames.push_back("Tealc"); 
  

  {
    // separate block to get delete called on object
    pNetworkInterface_t pfn(new FakeNetwork()); 

    recorder::pExperiment_t pExp =
      recorder::H5Experiment::create(pfn, filename); 
    
    typedef std::vector<recorder::pEpoch_t> evect_t; 

    for (svect_t::iterator name = epochnames.begin(); 
	 name != epochnames.end(); name++) {
      recorder::pEpoch_t epoch  = pExp->createEpoch(*name); 
    }
    
    pExp->close(); 
  }
  
  // now try and reload
  {
    pNetworkInterface_t pfn(new FakeNetwork()); 

    recorder::pExperiment_t pExp =
      recorder::H5Experiment::open(pfn, filename); 
    BOOST_CHECK_EQUAL(pExp->getEpochs().size(), epochnames.size()); 

    for (int i = 0; i < epochnames.size(); i++) {
      BOOST_CHECK_EQUAL(pExp->getEpochs()[i]->getName(), epochnames[i]); 
      
    }
  }
  
  
}


BOOST_AUTO_TEST_CASE(H5Experiment_epochrename )
{
  /*
    1. Can we rename epochs? 
    2. Do we trigger an error when we try and rename one to an existing
       name? 

  */
  
  std::string filename = "H5Experiment_epochrename.h5"; 
  filesystem::remove_all(filename); 
  {
  // separate block to get delete called on object
  pNetworkInterface_t pfn(new FakeNetwork()); 

  recorder::pExperiment_t pExp =
    recorder::H5Experiment::create(pfn, filename); 
  
  typedef std::vector<std::string > svect_t;
  typedef std::vector<recorder::pEpoch_t> evect_t; 

  svect_t epochnames; 
  evect_t epochs; 

  epochnames.push_back("Sam"); 
  epochnames.push_back("Jack"); 
  epochnames.push_back("Daniel"); 
  epochnames.push_back("Tealc"); 
  
  
  for (svect_t::iterator name = epochnames.begin(); 
       name != epochnames.end(); name++) {
    
    recorder::pEpoch_t epoch  = pExp->createEpoch(*name); 
    epochs.push_back(epoch); 
    BOOST_CHECK_EQUAL(epoch->getName(), *name); 
    
  }
  
  BOOST_CHECK_EQUAL(pExp->getEpochs().size(), epochnames.size()); 
  
  // now a rename
  epochnames[2] = "Daniel2"; 
  pExp->renameEpoch(epochs[2], epochnames[2]); 
  
  // Now check if we have returned them in the right order, with the right
  // names
  for (int i = 0; i < epochnames.size(); i++) {
    BOOST_CHECK_EQUAL(pExp->getEpochs()[i], epochs[i]);
    BOOST_CHECK_EQUAL(pExp->getEpochs()[i]->getName(), epochnames[i]);
  }

  //------------------------------------------------------
  // Conflicting rename check
  //------------------------------------------------------

  BOOST_CHECK_THROW(pExp->renameEpoch(epochs[1], "Sam"), std::runtime_error);  
  


  for (int i = 0; i < epochnames.size(); i++) {
    recorder::pEpoch_t epoch = pExp->getEpoch(epochnames[i]); 
    pExp->deleteEpoch(epoch); 
  }

  BOOST_CHECK_EQUAL(pExp->getEpochs().size(), 0); 
  
  // save the file
  pExp->close(); 
  }
  // now try and reopen it
  H5::H5File newfile; 
  newfile.openFile(filename, H5F_ACC_RDWR); 
  // now count the number of children -- should be just the notes
  BOOST_CHECK_EQUAL(newfile.getNumObjs(), 1); 

}

BOOST_AUTO_TEST_SUITE_END(); 
