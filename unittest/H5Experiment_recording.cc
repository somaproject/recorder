#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <iostream>   
#include <boost/shared_ptr.hpp>
#include <somanetwork/fakenetwork.h>
#include <somanetwork/tspike.h>
#include "experiment.h"  
#include "epoch.h"                   
#include "h5experiment.h"

using namespace soma; 
using namespace boost;       
using namespace boost::filesystem; 

BOOST_AUTO_TEST_SUITE(H5ExperimentRecording); 


pDataPacket_t createSpike(uint64_t time, datasource_t src) {
  // generate -very- homogeneous spike packets
  TSpike_t spike; 
  spike.src = src;
  spike.time = time; 
  
  TSpikeWave_t * wp[] = {&spike.x, &spike.y, &spike.a, &spike.b}; 
  
  for(int i = 0; i < 4; i++) {
    wp[i]->srcchan = i; 
    wp[i]->valid = 1;
    wp[i]->filtid = 7; 
    wp[i]->threshold=13;
    for (int j = 0; j < TSPIKEWAVE_LEN; j++) {
      wp[i]->wave[j] = j; 
    }
    
  }
  return rawFromTSpike(spike); 

}

BOOST_AUTO_TEST_CASE(H5ExperimentRecording_simplesave )
{
  /*
    Can we simply save data? 
  */
  
  std::string filename = "H5ExperimentRecording_simplesave.h5"; 
  filesystem::remove_all(filename); 

  // separate block to get delete called on object
  pNetworkInterface_t pfn(new FakeNetwork()); 
  recorder::pExperiment_t pExp = recorder::H5Experiment::create(pfn, filename); 
  
  recorder::pEpoch_t epoch  = pExp->createEpoch("Rose"); 
  epoch->enableDataSink(0, TSPIKE); 
  epoch->setDataName(0, "Sink0"); 
  
  pExp->dispatchData(createSpike(0, 0)); 
  
  // now start recording
  epoch->startRecording(); 
  for (int i = 0; i < 10; i++) {
    pExp->dispatchData(createSpike(i + 1000, 0)); 
  }
  epoch->stopRecording(); 
  pExp->close(); 

  int retval = system("python H5Experiment_recording.py simplesave");
  BOOST_CHECK_EQUAL(retval , 0); 

}

BOOST_AUTO_TEST_CASE(H5ExperimentRecording_saveswitch )
{
  /*
    Can we simply save and switch? 
  */
  
  std::string filename = "H5ExperimentRecording_saveswitch.h5"; 
  filesystem::remove_all(filename); 

  // separate block to get delete called on object
  pNetworkInterface_t pfn(new FakeNetwork()); 
  recorder::pExperiment_t pExp = recorder::H5Experiment::create(pfn, filename); 
  
  recorder::pEpoch_t epochR  = pExp->createEpoch("Rose"); 
  epochR->enableDataSink(0, TSPIKE); 
  epochR->setDataName(0, "Sink0"); 
  
  recorder::pEpoch_t epochD  = pExp->createEpoch("Donna"); 
  epochD->enableDataSink(0, TSPIKE); 
  epochD->setDataName(0, "Sink0"); 
  
  
  // now start recording
  epochR->startRecording(); 
  for (int i = 0; i < 10; i++) {
    pExp->dispatchData(createSpike(i + 1000, 0)); 
  }
  epochD->startRecording(); 
  for (int i = 10; i < 20; i++) {
    pExp->dispatchData(createSpike(i + 1000, 0)); 
  }
  

  epochD->stopRecording(); 
  pExp->close(); 

  int retval = system("python H5Experiment_recording.py saveswitch");
  BOOST_CHECK_EQUAL(retval , 0); 

}


BOOST_AUTO_TEST_CASE(H5ExperimentRecording_datasets )
{
  /*
    Do we correctly turn on and turn off datasets when
    switching epochs?
  */
  
  std::string filename = "H5ExperimentRecording_datasets.h5"; 
  filesystem::remove_all(filename); 

  pNetworkInterface_t pfn(new FakeNetwork()); 
  recorder::pExperiment_t pExp = recorder::H5Experiment::create(pfn, filename); 

  
  recorder::pEpoch_t epochR  = pExp->createEpoch("Rose"); 
  epochR->enableDataSink(0, TSPIKE); 
  epochR->setDataName(0, "Sink0"); 
  epochR->enableDataSink(1, TSPIKE); 
  epochR->setDataName(1, "Sink1"); 
  epochR->enableDataSink(2, TSPIKE); 
  epochR->setDataName(2, "Sink2"); 
  
  recorder::pEpoch_t epochD  = pExp->createEpoch("Donna"); 
  epochD->enableDataSink(2, TSPIKE); 
  epochD->setDataName(2, "Sink2"); 
  epochD->enableDataSink(3, TSPIKE); 
  epochD->setDataName(3, "Sink3"); 
  epochD->enableDataSink(4, TSPIKE); 
  epochD->setDataName(4, "Sink4"); 
  
  
  // now start recording
  epochR->startRecording(); 
  for (int i = 0; i < 10; i++) {
    for(int j = 0; j < 6; j++) {
      pExp->dispatchData(createSpike(i + 1000, j)); 
    }
  }
  epochD->startRecording(); 
  for (int i = 10; i < 20; i++) {
    for(int j = 0; j < 6; j++) {
      pExp->dispatchData(createSpike(i + 1000, j)); 
    }
  }
  

  epochD->stopRecording(); 
  pExp->close(); 

  int retval = system("python H5Experiment_recording.py datasets");
  BOOST_CHECK_EQUAL(retval , 0); 

}


BOOST_AUTO_TEST_CASE(H5ExperimentRecording_sessiontest )
{
  /*
    When we save data do we correctly save the session information? 

  */
  
  std::string filename = "H5ExperimentRecording_sessiontest.h5"; 
  filesystem::remove_all(filename); 

  // separate block to get delete called on object
  pNetworkInterface_t pfn(new FakeNetwork()); 
  recorder::pExperiment_t pExp = recorder::H5Experiment::create(pfn, filename); 
  
  recorder::pEpoch_t epoch  = pExp->createEpoch("Rose"); 
  epoch->enableDataSink(0, TSPIKE); 
  epoch->setDataName(0, "Sink0"); 
  
  pExp->dispatchData(createSpike(0, 0)); 

  recorder::pH5Experiment_t pH5Exp(boost::dynamic_pointer_cast<recorder::H5Experiment>(pExp)); 
  
  // now start recording
  pH5Exp->currentTS_ = 0x1234; 
  pH5Exp->currentTime_ = 100000; 
  
  epoch->startRecording(); 
  for (int i = 0; i < 10; i++) {
    pExp->dispatchData(createSpike(i + 1000, 0)); 
  }

  // FIXME ugly hack
  pH5Exp->currentTS_ = 0x1234 * 2; 
  pH5Exp->currentTime_ = 100000 * 2; 

  epoch->stopRecording(); 

  pH5Exp->currentTS_ = 0x11223344;
  pH5Exp->currentTime_ = 8000000; 

  epoch->startRecording(); 

  pH5Exp->currentTS_ = 0x55667788;
  pH5Exp->currentTime_ = 10000000; 

  epoch->stopRecording(); 

  pExp->close(); 

  int retval = system("python H5Experiment_recording.py sessiontest");
  BOOST_CHECK_EQUAL(retval , 0); 

}



BOOST_AUTO_TEST_CASE(H5ExperimentRecording_statstest )
{
  /*
    can we correctly save the data sink stats? 

  */
  
  std::string filename = "H5ExperimentRecording_stats.h5"; 
  filesystem::remove_all(filename); 

  // separate block to get delete called on object
  pFakeNetwork_t pfn(new FakeNetwork()); 
  recorder::pExperiment_t pExp = recorder::H5Experiment::create(pfn, filename); 
  
  recorder::pEpoch_t epoch  = pExp->createEpoch("Rose"); 
  epoch->enableDataSink(0, TSPIKE); 
  epoch->setDataName(0, "Sink0"); 
  
  pExp->dispatchData(createSpike(0, 0)); 

  recorder::pH5Experiment_t pH5Exp(boost::dynamic_pointer_cast<recorder::H5Experiment>(pExp)); 
  
  // now start recording
  pH5Exp->currentTS_ = 0x1234; 
  pH5Exp->currentTime_ = 100000; 
  
  epoch->startRecording(); 
  for (int i = 0; i < 10; i++) {
    pExp->dispatchData(createSpike(i + 1000, 0)); 
  }

  // FIXME ugly hack
  pH5Exp->currentTS_ = 0x1234 * 2; 
  pH5Exp->currentTime_ = 100000 * 2; 

  // set the data sink stats
  std::vector<DataReceiverStats> stats; 
  DataReceiverStats sink0stats; 
  sink0stats.source = 0; 
  sink0stats.type = 0; 
  sink0stats.pktCount = 0x1234; 
  stats.push_back(sink0stats); 
  pfn->setDataStats(stats); 

  epoch->stopRecording(); 

  pExp->close(); 

  int retval = system("python H5Experiment_recording.py statstest");
  BOOST_CHECK_EQUAL(retval , 0); 

}



BOOST_AUTO_TEST_CASE(H5ExperimentRecording_epochrename )
{
  /*
    Verify that epoch renaming works while we're recording 
    into a different epoch, and that renaming attempts
    into an epoch which we're currently using trhows an error
  */
  
  std::string filename = "H5ExperimentRecording_epochrename.h5"; 
  filesystem::remove_all(filename); 

  pNetworkInterface_t pfn(new FakeNetwork()); 
  recorder::pExperiment_t pExp = recorder::H5Experiment::create(pfn, filename); 

  
  recorder::pEpoch_t epochR  = pExp->createEpoch("Rose"); 
  epochR->enableDataSink(0, TSPIKE); 
  epochR->setDataName(0, "Sink0"); 
  epochR->enableDataSink(1, TSPIKE); 
  epochR->setDataName(1, "Sink1"); 
  epochR->enableDataSink(2, TSPIKE); 
  epochR->setDataName(2, "Sink2"); 
  
  recorder::pEpoch_t epochD  = pExp->createEpoch("Donna"); 
  epochD->enableDataSink(0, TSPIKE); 
  epochD->setDataName(0, "Sink0"); 
  
  // now start recording
  epochR->startRecording(); 
  for (int i = 0; i < 10; i++) {
    for(int j = 0; j < 6; j++) {
      pExp->dispatchData(createSpike(i + 1000, j)); 
    }
  }
  BOOST_CHECK_THROW(pExp->renameEpoch(epochR, "Martha"), std::runtime_error); 


  pExp->renameEpoch(epochD, "DoctorDonna"); 

  epochD->startRecording(); 
  for (int i = 10; i < 20; i++) {
    for(int j = 0; j < 6; j++) {
      pExp->dispatchData(createSpike(i + 1000, j)); 
    }
  }
  

  epochD->stopRecording(); 
  pExp->close(); 

  int retval = system("python H5Experiment_recording.py epochrename");
  BOOST_CHECK_EQUAL(retval , 0); 

}


BOOST_AUTO_TEST_SUITE_END(); 
