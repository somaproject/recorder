
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <H5Cpp.h>
#include <stdexcept>
#include "h5experiment.h"
#include "h5helper.h"
#include "h5epoch.h"
#include "epoch.h"

using namespace soma::recorder; 


pExperiment_t H5Experiment::open(pNetworkInterface_t pn, filename_t expfilename) {
  // named constructor to open an existing file
  
  // check for existence
  boost::filesystem::path p(expfilename); 

  if (not boost::filesystem::exists(p) )
    {
      boost::format errormsg("File %s does not exist." ); 
      throw std::runtime_error(boost::str(errormsg % expfilename) ) ; 
    }
  
  // try and open the file
  H5::H5File file; 
  file.openFile(expfilename, H5F_ACC_RDWR); 

  // pass to the real constructor
  pH5Experiment_t h5e(new H5Experiment(pn, file)); 
  
  h5e->filename_ = expfilename; 
  // get all the epochs
  int N = file.getNumObjs(); 
  for (int i = 0; i < N; i++) {
    std::string name = file.getObjnameByIdx(i); 
    if (name == "/notes") {
      // Per experiment notes

    } else {
      // must be an epoch?  FIXME: Have epoch attribute
      H5::Group g = file.openGroup(name); 
      pH5Epoch_t epoch = H5Epoch::open(pH5ExperimentWeak_t(h5e), 
				       g); 
      h5e->epochs_[epoch->getName()] = epoch; 

      // insert the epoch group in order
      h5e->epochsOrdered_[epoch->getEpochOrderID()] = epoch; 
      
    }
    
  }

  H5::Group root = file.openGroup("/"); 

  pNoteTable_t nt; 
  try {
    nt = NoteTable::open(root); 
  } catch (Exception & e) {
    h5e->logexperiment_.errorStream() << "Cannot find notes table in file"
				      << h5helper::getPath(root)
				      << " creating notes table"; 
    nt = NoteTable::create(root); 
  }
  
  h5e->pNoteTable_ = nt; 

  return boost::dynamic_pointer_cast<Experiment>(h5e); 
}


std::map<string, string> H5Experiment::getFileProperties()
{
  
  
}


pExperiment_t H5Experiment::create(pNetworkInterface_t pn, filename_t name) {
  // named constructor to create a new file
  boost::filesystem::path p(name); 
  
  if (boost::filesystem::exists(p) )
    {
      boost::format errormsg("File %s already exists." ); 
      throw std::runtime_error(boost::str(errormsg % name) ) ; 
    }
  

  H5::H5File file(name, H5F_ACC_TRUNC); 

  // set the file root group
  H5::Group root = file.openGroup("/"); 
  H5::DataSpace attrspace;
  H5::Attribute attr = root.createAttribute("epochOrderID", H5::PredType::NATIVE_INT, attrspace); 
  int x = 0; 
  attr.write(H5::PredType::NATIVE_INT, &x); 

  // pass to the real constructor
  pH5Experiment_t h5exp(new H5Experiment(pn, file)); 
  h5exp->filename_ = name; 

  // note table
  h5exp->pNoteTable_ = NoteTable::create(root); 

  return pExperiment_t(h5exp); 
  
}

H5Experiment::H5Experiment(pNetworkInterface_t pn, H5::H5File file) : 
  currentTS_(0), 
  currentTime_(0), 
  h5file_(file), 
  pNetwork_(pn), 
  currentTargetEpoch_(),
  isRecording_(false), 
  isClosed_(false), 
  logexperiment_(log4cpp::Category::getInstance("soma.recording.experiment"))

{
  
  
  // set up the event and data callbacks for the GLIB interface
  int data_fd = pNetwork_->getDataFifoPipe(); 

  
  Glib::signal_io().connect(sigc::mem_fun(*this, &H5Experiment::dataRXCallback), 
			    data_fd, Glib::IO_IN, Glib::PRIORITY_HIGH); 

  int event_fd =  pNetwork_->getEventFifoPipe(); 
  Glib::signal_io().connect(sigc::mem_fun(*this, &H5Experiment::eventRXCallback), 
			    event_fd, Glib::IO_IN, Glib::PRIORITY_HIGH); 


  // The network object is always running, but we do selectively 
  // enable and disable the glib callbacks! 

  pNetwork_->run(); 
  

}


void H5Experiment::close() {
  // close all of the epochs
  logexperiment_.infoStream() << "closing experiment at ts=" << getCurrentTS(); 
  epochsOrdered_.clear(); 
  epochs_.clear(); 

  logexperiment_.infoStream() << "now closing experiment file"; 
  h5file_.flush(H5F_SCOPE_LOCAL);

  h5file_.close();   

  pNetwork_->shutdown(); 

  isClosed_ =true; 
}

H5Experiment::~H5Experiment() {
  if (not isClosed_) {
    logexperiment_.warn("H5Experiment destructor called without explicit invocation of close"); 
    close(); 
  }

}


pEpoch_t H5Experiment::createEpoch(epochname_t name) {
  if (epochs_.find(name) != epochs_.end()) {
    throw std::runtime_error("Key already exists"); // FIXME BETTER ERROR
  } 

  // create epoch group

  int nextid = getNextEpochOrderID(); 
  pH5Epoch_t epoch = H5Epoch::create(shared_from_this(), h5file_.openGroup("/"),
				     name, nextid); 

  epochsOrdered_[epoch->getEpochOrderID()] = epoch; 
  epochs_[name]  = epoch; 
  return epoch; 
}



  


void H5Experiment::deleteEpoch(pEpoch_t epoch) {
  
  epochname_t name = epoch->getName(); 
  pH5Epoch_t h5epoch = epochs_[name]; 
  epochs_.erase(name); 
  for (epochordermap_t::iterator ei = epochsOrdered_.begin(); 
       ei != epochsOrdered_.end(); ei++) {
    if(ei->second == h5epoch) {
      epochsOrdered_.erase(ei); 
      h5file_.unlink("/" + h5epoch->getName()); 
      break;
    }
  }
  
  // now delete the actual H5 instance

  
}

pEpoch_t H5Experiment::getEpoch(epochname_t name) {
  return epochs_[name]; 

}

std::vector<pEpoch_t> H5Experiment::getEpochs() {
  std::vector<pEpoch_t> epochs; 
  epochs.reserve(epochsOrdered_.size()); 
  
  for(epochordermap_t::iterator i = epochsOrdered_.begin(); 
      i != epochsOrdered_.end(); i++) {
    boost::shared_ptr<H5Epoch> ptr = i->second; 

    epochs.push_back(boost::dynamic_pointer_cast<H5Epoch>(ptr)); 
  }

  return epochs; 
  
}

// returns the next epoch order ID
int H5Experiment::getNextEpochOrderID()
{
  H5::Group root = h5file_.openGroup("/"); 
  H5::Attribute attr = root.openAttribute("epochOrderID"); 
  int x; 
  attr.read(H5::PredType::NATIVE_INT, &x); 
  x++; 
  attr.write(H5::PredType::NATIVE_INT, &x); 
  return x; 
  
}

void H5Experiment::startRecording(epochname_t targetEpoch) {
  somatime_t currentts = getCurrentTS(); 
  int time = getCurrentTime(); 
  if (isRecording_) {
    std::map<dpair_t, SinkStats> ss; 
    currentTargetEpoch_->endSession(currentts, time, ss); 
  } 
  
  
  std::set<dpair_t> oldset, newset, turnoffset, turnonset; 
  
  newset =  epochs_[targetEpoch]->getDataSinks();

  if (currentTargetEpoch_) {
    oldset = currentTargetEpoch_->getDataSinks(); 
    

    
    set_difference(oldset.begin(), oldset.end(), 
		   newset.begin(), newset.end(), 
		   insert_iterator<std::set<dpair_t> >(turnoffset, turnoffset.begin())); 
    
    set_difference(newset.begin(), newset.end(), 
		   oldset.begin(), oldset.end(), 
		   insert_iterator<std::set<dpair_t> >(turnonset, turnonset.begin())); 
  } else {
    turnonset = newset; 
  }

    // now enable all relevant targets
  for (std::set<dpair_t>::iterator psink = turnonset.begin(); 
       psink != turnonset.end(); psink++) {
    pNetwork_->enableDataRX(psink->first, psink->second); 
    
  }
  
  for (std::set<dpair_t>::iterator psink = turnoffset.begin(); 
       psink != turnoffset.end(); psink++) {
    pNetwork_->enableDataRX(psink->first, psink->second); 
    
  }


  // and start
  currentTargetEpoch_ = epochs_[targetEpoch]; 
  currentTargetEpoch_->startSession(currentts, time); 
  isRecording_ = true; 
  
}

void H5Experiment::stopRecording() {

  somatime_t currentts = getCurrentTS(); 
  int time = getCurrentTime(); 

  
  if (isRecording_) {
    std::vector<DataReceiverStats> drstats = pNetwork_->getDataStats(); 
    std::map<dpair_t, SinkStats> sinkstats = convertStats(drstats); 
    currentTargetEpoch_->endSession(currentts, time, sinkstats); 
  } 

  // shutdown the network to disable all the data
  // and event receivers
  pNetwork_->disableAllDataRX(); 
  
  currentTargetEpoch_.reset(); 

  isRecording_ = false; 
  
}

std::map<dpair_t, SinkStats> H5Experiment::convertStats(const std::vector<DataReceiverStats> & stats)
{
  
  std::map<dpair_t, SinkStats> results; 
  std::vector<DataReceiverStats>::const_iterator s = stats.begin(); 
  for (; s != stats.end(); s++) {
    dpair_t key = std::make_pair((unsigned char)s->source, charToDatatype(s->type)); 
    
    SinkStats ss; 
    ss.pktCount = s->seqprotostats.rxPacketCount; 
    ss.latestSeq = s->seqprotostats.currentSequenceID; 
    ss.dupeCount = s->seqprotostats.dupeCount; 
    ss.missingPacketCount = s->seqprotostats.lostCount; 
    ss.reTxRxCount = s->seqprotostats.retxReqCount;  
    
    
    results[key] = ss; 
  }
  return results;
}


void H5Experiment::dispatchData(pDataPacket_t dp)
{
  logexperiment_.debug("data packet received"); 

  if (isRecording_) {
    currentTargetEpoch_->appendData(dp); 
  }

}


void H5Experiment::dispatchEvent(pEventPacket_t pep)
{
  if (isRecording_) {
    currentTargetEpoch_->appendEvent(pep); 
  }
  // update timestamps based on events
  if (!pep->missing) {
    EventList_t::iterator ei; 
    for(ei = pep->events->begin(); ei != pep->events->end(); ++ei){
      if (ei->cmd == 0x10 && ei->src == 0) {
	somatime_t st = 0; 
 	st = ei->data[0]; 
 	st = (st << 16) | ei->data[1]; 
 	st = (st << 16) | ei->data[2]; 
	st = (st << 16) | ei->data[3]; 	

	setCurrentTS(st); 
      }
    }
  }
  
}

somatime_t H5Experiment::getCurrentTS()
{
  return currentTS_; 

}

void H5Experiment::setCurrentTS(somatime_t ts)
{
  currentTS_ = ts; 
  if(currentTargetEpoch_) {
    currentTargetEpoch_->setCurrentTS(ts); 
  }
}

int H5Experiment::getCurrentTime()
{
  return currentTime_; 
}


bool H5Experiment::dataRXCallback(Glib::IOCondition io_condition)
{
  if ((io_condition & Glib::IO_IN) == 0) {
    logexperiment_.warn("dataRXcallback: invalid FIFO response"); 
    return false; 
  }
  else 
    {
      char x[100]; 
      int bytes = read(pNetwork_->getDataFifoPipe(), &x, 100); 
      for (int i = 0; i < bytes; i++) { 
	pDataPacket_t rdp = pNetwork_->getNewData(); 
      
	dispatchData(rdp); 
      }
      
    }
  return true; 
}


bool H5Experiment::eventRXCallback(Glib::IOCondition io_condition)
{
  
  if ((io_condition & Glib::IO_IN) == 0) {
    logexperiment_.warn("eventRXcallback: invalid FIFO response"); 

    return false; 
  }
  else 
    {
      char x; 
      read(pNetwork_->getEventFifoPipe(), &x, 1); 
      pEventPacket_t pep = pNetwork_->getNewEvents(); 
      dispatchEvent(pep); 
    }
  return true; 
}



std::string H5Experiment::getName()
{
  return filename_; 
}


void H5Experiment::renameEpoch(pEpoch_t epoch, epochname_t newname)
{
  // are we renaming to the same name? if so, just return
  if(  epoch->getName() == newname) {
    logexperiment_.infoStream()  << "rename of epoch " << newname << " to same name"; 
    return; 
  }
  // check if target name exists -- we should probably use epochs_
  std::vector<pEpoch_t> epochs = getEpochs(); 
  for (std::vector<pEpoch_t>::iterator i = epochs.begin();
       i != epochs.end(); ++i)
    {
      if( (*i)->getName() == newname) {
	throw std::runtime_error("Name already in use"); 
      }
    }
  
  // get a H5Epoch pointer to work with 
  pH5Epoch_t ph5e = boost::dynamic_pointer_cast<H5Epoch>(epoch);

  if((isRecording_) and  (currentTargetEpoch_== ph5e)) {
    throw(std::runtime_error("You are currently recording with this epoch, and cannot rename it")); 
  }

  // if not, go ahead
  epochname_t oldname = ph5e->getName(); 
  
  std::string oldpath = boost::str(boost::format("/%s") % oldname); 
  std::string newpath = boost::str(boost::format("/%s") % newname); 

  h5file_.move(oldpath, newpath); 
  
  epochs_.erase(oldname); 
  epochs_[newname] = ph5e; 
  logexperiment_.infoStream()  << "rename of epoch from " << oldname
			       << " to " << newname; 
    
}

notehandle_t H5Experiment::createNote() 
{
  return pNoteTable_->createNote(); 
}

Note_t H5Experiment::getNote(notehandle_t nid)
{
  return pNoteTable_->getNote(nid); 

}

std::list<notehandle_t> H5Experiment::getNotes()
{
  return pNoteTable_->getNotes(); 

}

void H5Experiment::setNote(const Note_t & note)
{
  pNoteTable_->setNote(note); 
}

void H5Experiment::deleteNote(notehandle_t nid)
{
  pNoteTable_->deleteNote(nid); 

}

