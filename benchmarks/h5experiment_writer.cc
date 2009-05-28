#include <iostream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <iostream>   

#include <fstream>
#include <boost/timer.hpp>

#include <boost/shared_ptr.hpp>
#include <somanetwork/fakenetwork.h>
#include <somanetwork/tspike.h>
#include "experiment.h"  
#include "epoch.h"                   
#include "h5experiment.h"

namespace bf = boost::filesystem; 

int main()
{
  /*
    Write some large number of tspikes in an epoch
    and measure the resulting performance. 

   */ 
  using namespace soma::recorder; 

  std::string filename = "H5Experiment_filecreate.h5"; 
  bf::remove_all(filename); 
  
  pFakeNetwork_t pfn(new FakeNetwork()); 
  //  pNetworkInterface_t pn(pfn); 
  pExperiment_t exp = H5Experiment::create(pfn, filename); 
  pEpoch_t epoch =  exp->createEpoch("Test"); 

  int TSPIKE_SINK_NUM = 64; 
  int TSPIKE_COUNT = 10000; 
  /* precreate all of the data so it doesn't color our benchmark time
   */ 
  typedef std::vector<TSpike_t> tspike_vector_t; 
  std::vector<tspike_vector_t> alltspikes; 
  float bytes(0.0); 
  for (int i = 0; i < TSPIKE_SINK_NUM; i++) {
    tspike_vector_t tsv(TSPIKE_COUNT); 
    for (int j = 0; j < TSPIKE_COUNT; j++) {
      tsv[j].src = i; 
      for (int k = 0; k < 32; k++) {
	tsv[j].x.wave[k] = j *32 + k; 
	tsv[j].y.wave[k] = j *32 + k + 10; 
	tsv[j].a.wave[k] = j *32 + k + 20; 
	tsv[j].b.wave[k] = j *32 + k + 30; 
      }
    }
    alltspikes.push_back(tsv); 
  }
  bytes += sizeof(TSpike_t) * TSPIKE_COUNT * TSPIKE_SINK_NUM; 
  
  for (int i = 0; i < TSPIKE_SINK_NUM; i++) {
    epoch->enableDataSink(i, TSPIKE); 
  }
  
  epoch->startRecording(); 
  boost::timer t;   
  for (int j = 0; j < TSPIKE_COUNT; j++) {
    for (int i = 0; i < TSPIKE_SINK_NUM; i++) {
      pDataPacket_t pd = somanetwork::rawFromTSpike(alltspikes[i][j]); 
      exp->dispatchData(pd); // pfn->appendDataOut(pd); 
    }
  }
  epoch->stopRecording(); 
  exp->close(); 
  double t_elapsed = t.elapsed(); 
  std::cout << "wrote " << bytes  << "bytes of raw data" << std::endl; 
  std::cout << "duration = " << t_elapsed << std::endl;
  std::cout << bytes/1e6 / t_elapsed << " MB/sec" << std::endl;

}
