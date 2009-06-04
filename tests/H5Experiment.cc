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
#include "test_config.h"
#include "test_util.h"

using namespace soma; 
using namespace boost;       
using namespace boost::filesystem; 

BOOST_AUTO_TEST_SUITE(H5Experiment); 


BOOST_AUTO_TEST_CASE(H5Experiment_filecreate )
{
  // Test if file creation works; 
  
  std::string filename = "H5Experiment_filecreate.h5"; 
  path h5filepath = test_binary_path / filename; 

  filesystem::remove_all(h5filepath); 
  {
    // separate block to get delete called on object
    pNetworkInterface_t pfn(new FakeNetwork()); 
    recorder::H5Experiment::create(pfn, h5filepath); 

  }
  // confirm existence of file
  BOOST_CHECK( exists(h5filepath) );
  
}


BOOST_AUTO_TEST_CASE(H5FileInterface_filecreateexists )
{
  // Test if file creation fails when the file exists
  
  std::string filename = "H5Experiment_filecreate_exist.h5"; 
  path h5filepath = test_binary_path / filename; 
  filesystem::remove_all(h5filepath); 
  // create the file
  std::ofstream file(h5filepath.string().c_str() );
  file << "test output"; 
  file.close(); 
  pNetworkInterface_t pfn(new FakeNetwork()); 
    
  BOOST_CHECK_THROW(recorder::H5Experiment::create(pfn, h5filepath), 
		    std::runtime_error); 
}

BOOST_AUTO_TEST_CASE(H5Experiment_fileopen )
{
  /*
    FIXME: bring back this test; for the time being we're not worrying
    about being able to open things from here. 

   */
//   //H5Eset_auto2(H5E_DEFAULT, 0, 0); 

//   // Test if file opening works
  
//   std::string filename = "H5Experiment_fileopen.h5"; 
//   path h5filepath = test_binary_path / filename; 
//   H5::H5File * h5f = new H5::H5File(h5filepath.string(),  H5F_ACC_TRUNC); 
//   H5::Group rootg = h5f->openGroup("/");
//   rootg.createGroup("experiment"); 
//   h5f->flush(H5F_SCOPE_LOCAL); 
//   h5f->close(); 
//   delete h5f; 

  
//   BOOST_CHECK( exists(h5filepath) ); // verify our create was successful. 
//   // separate block to get delete called on object

//   pNetworkInterface_t pfn(new FakeNetwork()); 

//   recorder::pExperiment_t pFI =
//     recorder::H5Experiment::open(pfn, h5filepath); 
  
//   path h5fileerr = test_binary_path / (filename + "ERROR"); 
//   BOOST_CHECK_THROW(recorder::H5Experiment::open(pfn, h5fileerr), 
// 		    std::runtime_error)
  
}


BOOST_AUTO_TEST_CASE(H5FileInterface_fileopennotexists)
{
  // Test if file open fails when the file doesn't exist
  
  std::string filename = "H5FileInterface_fileopen.h5"; 
  path h5filepath = test_binary_path / filename; 

  filesystem::remove_all(h5filepath); 
  
  pNetworkInterface_t pfn(new FakeNetwork()); 

  BOOST_CHECK_THROW(recorder::H5Experiment::open(pfn, h5filepath), 
		    std::runtime_error); 
}
		     


BOOST_AUTO_TEST_CASE(H5Experiment_timestamp )
{
  /*
    Check that we can get and set the reference time


   */
  
  std::string filename = "H5Experiment_reftime.h5"; 
  path h5filepath = test_binary_path / filename; 

  filesystem::remove_all(h5filepath); 
  
  {
    // separate block to get delete called on object
    pNetworkInterface_t pfn(new FakeNetwork()); 
    pExperiment_t exp = recorder::H5Experiment::create(pfn, h5filepath); 
    
    // initially should be zero? 
    BOOST_CHECK_EQUAL(exp->getReferenceTime(), 0); 
    exp->setReferenceTime(0x12345678); 
    
    BOOST_CHECK_EQUAL(exp->getReferenceTime(), 0x12345678); 
  }
  
  int retval = run_standard_py_script("H5Experiment.py", 
				      "timestamp"); 
  BOOST_CHECK_EQUAL(retval , 0); 

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
  path h5filepath = test_binary_path / filename; 
  filesystem::remove_all(h5filepath); 

  // separate block to get delete called on object
  pNetworkInterface_t pfn(new FakeNetwork()); 

  recorder::pExperiment_t pExp =
    recorder::H5Experiment::create(pfn, h5filepath); 
  
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
  newfile.openFile(h5filepath.string(), H5F_ACC_RDWR); 
  // now count the number of children -- should just be notes, i.e. 1
  BOOST_CHECK_EQUAL(newfile.getNumObjs(), 1); 

}

// BOOST_AUTO_TEST_CASE(H5Experiment_epochload )
// {
//   /*
//     1. create multiple epochs
//     2. close and reopen file
//     3. check we have at least superficially recovered the epochs
//   */
  
//   std::string filename = "H5Experiment_epochreload.h5"; 
//   filesystem::remove_all(filename); 
    
//   typedef std::vector<std::string > svect_t;
  
//   svect_t epochnames; 
  
//   epochnames.push_back("Sam"); 
//   epochnames.push_back("Jack"); 
//   epochnames.push_back("Daniel"); 
//   epochnames.push_back("Tealc"); 
  

//   {
//     // separate block to get delete called on object
//     pNetworkInterface_t pfn(new FakeNetwork()); 

//     recorder::pExperiment_t pExp =
//       recorder::H5Experiment::create(pfn, filename); 
    
//     typedef std::vector<recorder::pEpoch_t> evect_t; 

//     for (svect_t::iterator name = epochnames.begin(); 
// 	 name != epochnames.end(); name++) {
//       recorder::pEpoch_t epoch  = pExp->createEpoch(*name); 
//     }
    
//     pExp->close(); 
//   }
  
//   // now try and reload
//   {
//     pNetworkInterface_t pfn(new FakeNetwork()); 

//     recorder::pExperiment_t pExp =
//       recorder::H5Experiment::open(pfn, filename); 
//     BOOST_CHECK_EQUAL(pExp->getEpochs().size(), epochnames.size()); 

//     for (int i = 0; i < epochnames.size(); i++) {
//       BOOST_CHECK_EQUAL(pExp->getEpochs()[i]->getName(), epochnames[i]); 
      
//     }
//   }
  
  
// }


BOOST_AUTO_TEST_CASE(H5Experiment_epochrename )
{
  /*
    1. Can we rename epochs? 
    2. Do we trigger an error when we try and rename one to an existing
       name? 

  */
  
  std::string filename = "H5Experiment_epochrename.h5"; 
  path h5filepath = test_binary_path / "TSpikeTable_create.h5"; 

  filesystem::remove_all(h5filepath); 
  {
  // separate block to get delete called on object
  pNetworkInterface_t pfn(new FakeNetwork()); 

  recorder::pExperiment_t pExp =
    recorder::H5Experiment::create(pfn, h5filepath); 
  
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
  newfile.openFile(h5filepath.string(), H5F_ACC_RDWR); 
  // now count the number of children -- should be just the notes
  BOOST_CHECK_EQUAL(newfile.getNumObjs(), 1); 

}

BOOST_AUTO_TEST_SUITE_END(); 

