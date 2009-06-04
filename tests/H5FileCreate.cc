/*

Make a file with known data such that we can distribute it for later
use. We create two multimaps of event packets
and data packets, with associated times. 

I considered using a priority queue, but there's
really no need given that we know the ordering of
all data long before we extract anything from the queue. 


The event and data packet queues are filled with some appropriate collection
of data, and then we selectively enable the sources, and 
dispatch to them via the fake network interface by
_bruteforcing_ through the entire data range... 



*/


#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <iostream>   
#include <algorithm>
#include <fstream>


#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>

#include <boost/shared_ptr.hpp>
#include <somanetwork/fakenetwork.h>
#include <somanetwork/tspike.h>
#include <somanetwork/wave.h>
#include "experiment.h"  
#include "epoch.h"                   
#include "h5epoch.h"                   
#include "h5experiment.h"

using namespace soma::recorder; 

using namespace boost;       
using namespace boost::filesystem; 

BOOST_AUTO_TEST_SUITE(H5FileCreate); 
typedef uint64_t somatime_t; 
uint64_t timestamp=0;  

typedef std::multimap<somatime_t, pDataPacket_t> dataPacketQueue_t; 
typedef std::multimap<somatime_t, pEventPacket_t> eventPacketQueue_t; 
typedef std::vector<EventList_t> eventTimeline_t; 
void setupSleep1(pEpoch_t ep, pNetworkInterface_t net); 
void setupSleep2(pEpoch_t ep, pNetworkInterface_t net); 
void setupSleep3(pEpoch_t ep, pNetworkInterface_t net); 
void setupRun1(pEpoch_t ep, pNetworkInterface_t net); 
void setupRun2(pEpoch_t ep, pNetworkInterface_t net); 

somatime_t EVENTCYCLES_PER_SECOND = 50000; 

void createSpikeStream(dataPacketQueue_t *  pDPQ, somatime_t startts, 
		       somatime_t endts, datasource_t src, somatime_t period)
{
  /*
    Fill the time from startts to endts with packets every period ts. 

  */
  somatime_t curts = startts; 
  while (curts < endts) {
    TSpike_t ts; 
    ts.src = src; 
    ts.time = curts; 

    TSpikeWave_t * tswp[4]; 
    tswp[0] = &ts.x; 
    tswp[1] = &ts.y; 
    tswp[2] = &ts.a; 
    tswp[3] = &ts.b; 
    
    for (int j = 0; j < 4; j++) {
      TSpikeWave_t * tw = tswp[j]; 
      tw->filtid = 0x100;
      tw->valid = 1; 
      tw->threshold = 100000; 
      for (int k = 0; k < TSPIKEWAVE_LEN; k++) {
	tw->wave[k] = j + k; 
      }
    }
	
    pDataPacket_t rdp(rawFromTSpike(ts)); 
    pDPQ->insert(make_pair(curts, rdp)); 
    curts += period; 
  }

}
void createWaveStream(dataPacketQueue_t *  pDPQ, somatime_t startts, 
		       somatime_t endts, datasource_t src, int sampfreq, bool sine = false)
{
  /*
    generate a continuous series of wave stream packets
    beginning at startts; at sampfreq. 

  */
  somatime_t curts = startts; 
  double t = 0; 
  double freq = 100.0; 

  while (curts < endts) {
    Wave_t w; 
    w.src = src; 
    w.time = curts; 
    w.sampratenum = 2000; 
    w.samprateden = 1; 
    w.selchan = src % 4; 
    w.filterid = 0xAABBCCD; 

    for (int j = 0; j < WAVEBUF_LEN; j++) {    
      if (sine) {
	w.wave[j] = int32_t(round(1e6 * sin(t * 3.14 * 2 * freq))); 
	t += 1.0 / sampfreq; 
      } else { 
	w.wave[j] = src * 1000 + j;  // simple incremental counter
      }
    }
    pDataPacket_t rdp = rawFromWave(w); 

    pDPQ->insert(make_pair(curts, rdp));
    double pktdur_sec = double(WAVEBUF_LEN)/sampfreq; 
    curts += somatime_t(EVENTCYCLES_PER_SECOND * pktdur_sec); 
  }
  
}

void createTimeEvents(eventTimeline_t * etl, somatime_t startts)
{
  /*
    beginning at startts, put in a time event at every location
    in the timeline
    
  */
  
  int pos = 0; 
  for(somatime_t ts = startts; ts < startts + etl->size(); ++ts) {
    Event_t event; 

    // first the timer event
    event.cmd = 0x10; 
    event.src = 0; 
    event.data[0] = ts >> 48; 
    event.data[1] = ts >> 32; 
    event.data[2] = ts >> 16; 
    event.data[3] = ts & 0xFFFF; 
    
    (*etl)[pos].push_back(event); 
    pos++; 


  }

}

void eventTimelineToQueue(eventTimeline_t *etl, somatime_t startts, 
			  eventPacketQueue_t * epq)
{
  /* Somewhat intelligently walk the timeline and create event packets
     based upon our simple heuristics of packet size
  */ 
  
  int seq = 0; 
  pEventList_t totalel(new EventList_t); 
  int eventcycles = 0; 
  somatime_t curtime = startts; 
  for(eventTimeline_t::iterator etli = etl->begin(); etli != etl->end(); ++etli)
    {
      if ((totalel->size() +  etli->size()  > 80) or eventcycles > 10){ 
	// send the current one
	pEventPacket_t ep(new EventPacket_t); 
	ep->seq = seq; 
	ep->missing = false; 
	// map the events
	ep->events = totalel; 
	epq->insert(make_pair(curtime, ep)); 
	
	totalel->clear(); 
	seq++; 
      } 
      copy(etli->begin(), etli->end(), back_inserter(*totalel)); 
      curtime++; 
    }

  if(totalel->size() > 0) {
    pEventPacket_t ep(new EventPacket_t); 
    ep->seq = seq; 
    ep->missing = false; 
    // map the events
    ep->events = totalel; 
    epq->insert(make_pair(curtime, ep)); 
  }
}

BOOST_AUTO_TEST_CASE(H5FileCreate_main )
{
  // Test if file creation works; 
  
  log4cpp::Appender * logappender =  new log4cpp::OstreamAppender("appender", &std::cout); 
  log4cpp::Layout* loglayout = new log4cpp::BasicLayout();
  logappender->setLayout(loglayout); 
  //log4cpp::Category::getRoot().setPriority(log4cpp::Priority::DEBUG); 
  log4cpp::Category& root = log4cpp::Category::getRoot();
  
  root.addAppender(logappender);
 
  std::string filename = "H5FileCreate_main.h5"; 
  filesystem::remove_all(filename); 
  
  timestamp = 0; 

  pNetworkInterface_t pfn(new FakeNetwork()); 
  pExperiment_t exp = H5Experiment::create(pfn, filename); 


  int duration_secs =  200; 
  somatime_t startts = 0; 
  somatime_t endts = EVENTCYCLES_PER_SECOND * duration_secs; 
  
  // create event array
  eventTimeline_t * etl = new eventTimeline_t(endts - startts); 

  // create two queues
  dataPacketQueue_t * dpq = new dataPacketQueue_t;
  eventPacketQueue_t*  epq = new eventPacketQueue_t; 

  // generate fake data
  createSpikeStream(dpq, 0, endts, 0, 100); // 500 Hz. 
  createSpikeStream(dpq, 0, endts, 1, 200); // 250 Hz. 
  createSpikeStream(dpq, 0, endts, 2, 400); // 125 Hz. 
  createSpikeStream(dpq, 0, endts, 3, 800); // 62.5 Hz. 
  createSpikeStream(dpq, 0, endts, 4, 1000); // 50 Hz. 
  createSpikeStream(dpq, 0, endts, 5, 2000); // 25 Hz. 
  createSpikeStream(dpq, 0, endts, 6, 2000); // 25 Hz. 
  createSpikeStream(dpq, 0, endts, 7, 2000); // 25 Hz. 
  createSpikeStream(dpq, 0, endts, 8, 2000); // 25 Hz. 
  createSpikeStream(dpq, 0, endts, 9, 2000); // 25 Hz. 
  createSpikeStream(dpq, 0, endts, 10, 2000); // 25 Hz. 
  createSpikeStream(dpq, 0, endts, 11, 2000); // 25 Hz. 

  createWaveStream(dpq, 0, endts, 0, 1000); 
  createWaveStream(dpq, 0, endts, 1, 1000); 
  createWaveStream(dpq, 0, endts, 2, 2000, true); 
  createWaveStream(dpq, 0, endts, 3, 2000); 
  createWaveStream(dpq, 0, endts, 4, 4000, true); 
  createWaveStream(dpq, 0, endts, 5, 4000); 
  createWaveStream(dpq, 0, endts, 6, 4000); 
  
  createTimeEvents(etl, startts); 
  eventTimelineToQueue(etl, startts, epq);

  
  pEpoch_t epochSleep1 = exp->createEpoch("sleep1");  
  pEpoch_t epochRun1 = exp->createEpoch("run1");  
  pEpoch_t epochSleep2 = exp->createEpoch("sleep2");  
  pEpoch_t epochRun2 = exp->createEpoch("run2");  
  pEpoch_t epochSleep3 = exp->createEpoch("sleep3");  

  setupSleep1(epochSleep1, pfn); 
  setupRun1(epochRun1, pfn); 
  setupSleep2(epochSleep2, pfn); 
  setupRun2(epochRun2, pfn); 
  setupSleep3(epochSleep3, pfn); 

  // now, we set up the epochs along with start, stop times
  std::vector<pEpoch_t> epochs; 
  std::vector<somatime_t> epochStartTimes_sec; 
  epochs.push_back(epochSleep1); 
  epochStartTimes_sec.push_back(0); 

  epochs.push_back(epochRun1); 
  epochStartTimes_sec.push_back(10); 

  epochs.push_back(epochSleep2); 
  epochStartTimes_sec.push_back(40); 

  epochs.push_back(epochRun2); 
  epochStartTimes_sec.push_back(120); 

  epochs.push_back(epochSleep3); 
  epochStartTimes_sec.push_back(180); 
  
  // primary dispatch loop
  int currentepoch = 0; 
  for (int ts = startts; ts != endts; ts++) {
    if (currentepoch < epochs.size()){
      if (ts == (epochStartTimes_sec[currentepoch] * EVENTCYCLES_PER_SECOND ))
	{
	  epochs[currentepoch]->startRecording(); 
	  currentepoch++; 
	}
    }

    pair<eventPacketQueue_t::iterator, eventPacketQueue_t::iterator> epp;
    epp = epq->equal_range(ts); 
    for(eventPacketQueue_t::iterator i = epp.first; i != epp.second; ++i) {
      exp->dispatchEvent(i->second); 
      
    }


    pair<dataPacketQueue_t::iterator, dataPacketQueue_t::iterator> dpp;
    dpp = dpq->equal_range(ts); 
    for(dataPacketQueue_t::iterator i = dpp.first; i != dpp.second; ++i) {
      exp->dispatchData(i->second); 
    }


    // DISPATCH THE DATA!!!
  dataPacketQueue_t * dpq = new dataPacketQueue_t;
  eventPacketQueue_t*  epq = new eventPacketQueue_t; 

  }
  epochSleep3->stopRecording(); 
  
  exp->close(); 

}
BOOST_AUTO_TEST_CASE(H5FileCreate_small )
{
  // Test if file creation works; 
  
//   log4cpp::Appender * logappender =  new log4cpp::OstreamAppender("appender", &std::cout); 
//   log4cpp::Layout* loglayout = new log4cpp::BasicLayout();
//   logappender->setLayout(loglayout); 
//   log4cpp::Category::getRoot().setPriority(log4cpp::Priority::DEBUG); 
//   log4cpp::Category& root = log4cpp::Category::getRoot();
  
//   root.addAppender(logappender);
 
  std::string filename = "H5FileCreate_small.h5"; 
  filesystem::remove_all(filename); 
  
  timestamp = 0; 

  pNetworkInterface_t pfn(new FakeNetwork()); 
  pExperiment_t exp = H5Experiment::create(pfn, filename); 


  int duration_secs =  20; 
  somatime_t startts = 0; 
  somatime_t endts = EVENTCYCLES_PER_SECOND * duration_secs; 
  
  // create event array
  eventTimeline_t * etl = new eventTimeline_t(endts - startts); 

  // create two queues
  dataPacketQueue_t * dpq = new dataPacketQueue_t;
  eventPacketQueue_t*  epq = new eventPacketQueue_t; 

//   // generate fake data
//   createSpikeStream(dpq, 0, endts, 0, 100); // 500 Hz. 


  
//   createTimeEvents(etl, startts); 
//   eventTimelineToQueue(etl, startts, epq);

  
  pEpoch_t epochSleep1 = exp->createEpoch("sleep1");  

//   setupSleep1(epochSleep1, pfn); 

//   // now, we set up the epochs along with start, stop times
//   std::vector<pEpoch_t> epochs; 
//   std::vector<somatime_t> epochStartTimes_sec; 
//   epochs.push_back(epochSleep1); 
//   epochStartTimes_sec.push_back(0); 
  
//   epochSleep1->startRecording(); 
//   epochSleep1->stopRecording(); 
  
  exp->close(); 

}


void setupSimpleSleep(pEpoch_t ep, pNetworkInterface_t net) {

  // cast
  pH5Epoch_t h5ep = boost::dynamic_pointer_cast<H5Epoch>(ep); 

  h5ep->enableDataSink(0, TSPIKE); 
  h5ep->setDataName(0, "hippo0"); 
  
  h5ep->enableDataSink(1, TSPIKE); 
  h5ep->setDataName(1, "hippo1"); 
  
  h5ep->enableDataSink(2, TSPIKE); 
  h5ep->setDataName(2, "hippo2"); 
  
  h5ep->enableDataSink(3, WAVE); 
  h5ep->enableDataSink(3, TSPIKE); 
  h5ep->setDataName(3, "hippo3"); 
  
  h5ep->enableDataSink(4, WAVE); 
  h5ep->setDataName(4, "hippo44"); 
  
  h5ep->enableDataSink(5, WAVE); 
  h5ep->setDataName(5, "dg1"); 
    
  h5ep->enableDataSink(6, WAVE); 
  h5ep->setDataName(6, "dg2"); 
    

  
}
void setupSleep1(pEpoch_t ep, pNetworkInterface_t net) {

  // cast
  pH5Epoch_t h5ep = boost::dynamic_pointer_cast<H5Epoch>(ep); 

  h5ep->enableDataSink(0, TSPIKE); 
  h5ep->setDataName(0, "hippo0"); 
  
  h5ep->enableDataSink(1, TSPIKE); 
  h5ep->setDataName(1, "hippo1"); 
  
  h5ep->enableDataSink(2, TSPIKE); 
  h5ep->setDataName(2, "hippo2"); 
  
  h5ep->enableDataSink(3, WAVE); 
  h5ep->enableDataSink(3, TSPIKE); 
  h5ep->setDataName(3, "hippo3"); 
  
  h5ep->enableDataSink(4, WAVE); 
  h5ep->setDataName(4, "hippo44"); 
  
  h5ep->enableDataSink(5, WAVE); 
  h5ep->setDataName(5, "dg1"); 
    
  h5ep->enableDataSink(6, WAVE); 
  h5ep->setDataName(6, "dg2"); 
    

  
}

void setupRun1(pEpoch_t ep, pNetworkInterface_t net) {

  // cast
  pH5Epoch_t h5ep = boost::dynamic_pointer_cast<H5Epoch>(ep); 

  h5ep->enableDataSink(0, TSPIKE); 
  h5ep->setDataName(0, "hippo0"); 
  
  h5ep->enableDataSink(1, TSPIKE); 
  h5ep->setDataName(1, "hippo1"); 
  
  h5ep->enableDataSink(2, TSPIKE); 
  h5ep->setDataName(2, "hippo2"); 
  
  h5ep->enableDataSink(3, WAVE); 
  h5ep->setDataName(3, "hippo3"); 
    
  h5ep->enableDataSink(6, WAVE); 
  h5ep->setDataName(6, "dg2"); 
    

  
}

void setupSleep2(pEpoch_t ep, pNetworkInterface_t net) {

  // cast
  pH5Epoch_t h5ep = boost::dynamic_pointer_cast<H5Epoch>(ep); 

  h5ep->enableDataSink(0, TSPIKE); 
  h5ep->setDataName(0, "hippo0"); 
  
  h5ep->enableDataSink(1, TSPIKE); 
  h5ep->setDataName(1, "hippo1"); 
  
  h5ep->enableDataSink(2, TSPIKE); 
  h5ep->setDataName(2, "hippo2"); 
  
  h5ep->enableDataSink(3, TSPIKE); 
  h5ep->setDataName(3, "hippo3"); 
  
  h5ep->enableDataSink(4, TSPIKE); 
  h5ep->setDataName(4, "hippo4"); 
  
  h5ep->enableDataSink(5, TSPIKE); 
  h5ep->setDataName(5, "dg1"); 
    
  h5ep->enableDataSink(6, TSPIKE); 
  h5ep->setDataName(6, "dg2"); 
    

  
}

void setupSleep3(pEpoch_t ep, pNetworkInterface_t net) {

  // cast
  pH5Epoch_t h5ep = boost::dynamic_pointer_cast<H5Epoch>(ep); 

  h5ep->enableDataSink(0, TSPIKE); 
  h5ep->setDataName(0, "hippo0"); 
  
  h5ep->enableDataSink(1, TSPIKE); 
  h5ep->setDataName(1, "hippo1"); 
  
  h5ep->enableDataSink(2, TSPIKE); 
  h5ep->enableDataSink(2, WAVE); 
  h5ep->setDataName(2, "hippo2"); 
  
  h5ep->enableDataSink(3, WAVE); 
  h5ep->setDataName(3, "hippo3"); 
  
  h5ep->enableDataSink(4, WAVE); 
  h5ep->setDataName(4, "hippo4"); 
  
  h5ep->enableDataSink(5, WAVE); 
  h5ep->setDataName(5, "dg1"); 
    
  h5ep->enableDataSink(6, WAVE); 
  h5ep->setDataName(6, "dg2"); 
    

  
}

void setupRun2(pEpoch_t ep, pNetworkInterface_t net) {

  // cast
  pH5Epoch_t h5ep = boost::dynamic_pointer_cast<H5Epoch>(ep); 

  h5ep->enableDataSink(0, TSPIKE); 
  h5ep->setDataName(0, "hippo0"); 
  
  h5ep->enableDataSink(1, TSPIKE); 
  h5ep->setDataName(1, "hippo1"); 
  
  h5ep->enableDataSink(2, WAVE); 
  h5ep->setDataName(2, "hippo2"); 
  
  h5ep->enableDataSink(3, WAVE); 
  h5ep->setDataName(3, "hippo3"); 
  
  h5ep->enableDataSink(4, TSPIKE); 
  h5ep->setDataName(4, "hippo4"); 
  
  h5ep->enableDataSink(5, WAVE); 
  h5ep->setDataName(5, "dg1"); 
    
  h5ep->enableDataSink(6, WAVE); 
  h5ep->setDataName(6, "dg2"); 
    

  
}


BOOST_AUTO_TEST_SUITE_END(); 
