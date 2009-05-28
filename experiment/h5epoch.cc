#include <boost/format.hpp>
#include <exception>

#include "h5epoch.h"
#include "h5helper.h"
#include "tspiketable.h"
#include "wavetable.h"
#include "eventtable.h"


using namespace soma::recorder; 


pH5Epoch_t H5Epoch::create(pH5ExperimentWeak_t parent, 
			   H5::Group containingGroup, 
			   std::string name, int epochOrderID)
{
  H5::Group epochGroup = containingGroup.createGroup(name); 
  
  pH5Epoch_t pH5Epoch(new H5Epoch(parent, epochGroup)); 
  
  H5::Attribute attr = epochGroup.createAttribute("epochOrderID", 
						  H5::PredType::NATIVE_INT, 
						  H5::DataSpace());
  int x = epochOrderID; 
  attr.write(H5::PredType::NATIVE_INT, &x); 


  // create the events
  
  pH5Epoch->pEventTable_ = EventTable::create(epochGroup); 
  
  pH5Epoch->pNoteTable_ = NoteTable::create(epochGroup); 

  return pH5Epoch; 

}
  
pH5Epoch_t H5Epoch:: open(pH5ExperimentWeak_t parent, H5::Group epochGroup)
{


  pH5Epoch_t pH5Epoch(new H5Epoch(parent, epochGroup)); 

  // ---------------------------------------
  //   Event table open 
  // ---------------------------------------

  pEventTable_t et; 
  try {
    et = EventTable::open(epochGroup); 
    
  } catch (Exception & e) {
    pH5Epoch->logepoch_.errorStream() << "Cannot find Events table in epoch" 
				      << h5helper::getPath(epochGroup) 
				      << " Creating events table."; 
    et = EventTable::create(epochGroup); 
  }

  pH5Epoch->pEventTable_ = et; 

  pNoteTable_t nt; 
  try {
    nt = NoteTable::open(epochGroup);
  } catch (Exception & e) {
    pH5Epoch->logepoch_.errorStream() << "Cannot find notes table in epoch"
				      << h5helper::getPath(epochGroup)
				      << " creating notes table"; 
    nt = NoteTable::create(epochGroup); 
  }
  
  pH5Epoch->pNoteTable_ = nt; 
  
  // sessions / sessionCache
  pH5Epoch->sessionCache_= loadSessionAttribute(epochGroup); 

  for (int datatypegroupi = 0; datatypegroupi < epochGroup.getNumObjs(); 
       datatypegroupi++) {
    std::string name = epochGroup.getObjnameByIdx(datatypegroupi); 

    if (name == "TSpikes") {
      // ---------------------------------------
      //  TSpike groups and tables
      // ---------------------------------------
      pH5Epoch->haveTSpikeGroup_ = true; 
      pH5Epoch->h5TSpikeGroup_ = epochGroup.openGroup("TSpikes"); 
      
      // now for each item in the group, get the relevant data
      for (int tspikei = 0; tspikei < pH5Epoch->h5TSpikeGroup_.getNumObjs(); tspikei++) {
	std::string name = pH5Epoch->h5TSpikeGroup_.getObjnameByIdx(tspikei); 
	pTSpikeTable_t  ts = TSpikeTable::open(pH5Epoch->h5TSpikeGroup_, name); 
	
	datasource_t src = ts->getSource(); 
	dpair_t pair = std::make_pair(src, TSPIKE); 
	// Set up data dispatch table
	pH5Epoch->dispatchTable_[pair] = ts; 

	// Named sink table
	if (pH5Epoch->sourceToName_.find(src) == pH5Epoch->sourceToName_.end()){
	  pH5Epoch->sourceToName_[src] = name; 
	} else {
	  // source is currently in namedsinktable, should have the 
	  // same name
	  if (pH5Epoch->sourceToName_[src] != name) {
	    pH5Epoch->logepoch_.error("Data file is inconsistent, and has a data sources with multiple names"); 
	  }
	  
	}

	// sourceToSink table
	pH5Epoch->sourceToSink_.insert(std::make_pair(src, pDatasetIO_t(ts))); 

      }
    } else if (name == "Waves") {

      // ---------------------------------------
      //  Wave groups and tables
      // ---------------------------------------
      pH5Epoch->haveWaveGroup_ = true; 
      pH5Epoch->h5WaveGroup_ = epochGroup.openGroup("Waves"); 
      
      // now for each item in the group, get the relevant data
      for (int tspikei = 0; tspikei < pH5Epoch->h5WaveGroup_.getNumObjs(); tspikei++) {
	std::string name = pH5Epoch->h5WaveGroup_.getObjnameByIdx(tspikei); 
	pWaveTable_t  ts = WaveTable::open(pH5Epoch->h5WaveGroup_, name); 
	
	datasource_t src = ts->getSource(); 
	dpair_t pair = std::make_pair(src, TSPIKE); 
	// Set up data dispatch table
	pH5Epoch->dispatchTable_[pair] = ts; 
	
	// Named sink table
	if (pH5Epoch->sourceToName_.find(src) == pH5Epoch->sourceToName_.end()){
	  pH5Epoch->sourceToName_[src] = name; 
	} else {
	  // source is currently in namedsinktable, should have the 
	  // same name
	  if (pH5Epoch->sourceToName_[src] != name) {
	    pH5Epoch->logepoch_.error("Data file is inconsistent, and has a data sources with multiple names"); 
	  }
	  
	}
	
	// sourceToSink table
	pH5Epoch->sourceToSink_.insert(std::make_pair(src, pDatasetIO_t(ts))); 
	
	
      }
    }
    
  }
  
  
  return pH5Epoch; 
}

H5Epoch::H5Epoch(pH5ExperimentWeak_t parent, H5::Group epochGroup) :
  parentExperiment_(parent), 
  h5group_(epochGroup), 
  haveTSpikeGroup_(false), 
  haveWaveGroup_(false),
  isRecording_(false), 
  hasRecorded_(false),
  isClosed_(false),
  logepoch_(log4cpp::Category::getInstance("soma.recording.epoch"))
{

  
  //   pNoteTable_ = openNoteTable(); 
  currentSession_.startts = 0; 
  currentSession_.starttime = 0; 
  currentSession_.endts = 0; 
  currentSession_.endtime = 0; 
  
  logepoch_.info("H5Epoch::H5Epoch constructed"); 

}

void H5Epoch::close()
{
  h5TSpikeGroup_.close(); 
  h5WaveGroup_.close(); 
  pEventTable_->close(); 
  isClosed_=true; 
}

H5Epoch::~H5Epoch()
{
  if (not isClosed_){
    logepoch_.warn("H5Epoch::H5Epoch was not closed before destruction"); 
    close(); 
  }

  logepoch_.debug("H5Epoch::~H5Epoch (destructing)"); 
}


epochname_t H5Epoch::getName() 
{

  return h5helper::getName(h5group_); 

}

int H5Epoch::getEpochOrderID()
{
  H5::Attribute attr = h5group_.openAttribute("epochOrderID"); 
  int x; 
  attr.read(H5::PredType::NATIVE_INT, &x); 
  attr.close();
  return x; 
}

std::list<Session> H5Epoch::getSessions() 
{
  std::list<Session> seslist; 
  for(std::vector<Session>::iterator i = sessionCache_.begin(); 
      i != sessionCache_.end() ; i++) 
    {
      seslist.push_back(*i); 
    }
  if(isRecording_) {
    seslist.push_back(currentSession_); 
  }
  return seslist; 
}

void H5Epoch::startRecording(){
  logepoch_.debug("H5Epoch start recording"); 
  parentExperiment_.lock()->startRecording(getName()); 
  isRecording_ = true; 
  hasRecorded_ = true; 
}

void H5Epoch::stopRecording() {
  parentExperiment_.lock()->stopRecording(); 
  logepoch_.debug("H5Epoch stop recording"); 

  isRecording_ = false; 
}

void H5Epoch::startSession(somatime_t ts, int time)
{
  currentSession_.startts = ts; 
  currentSession_.starttime = time; 
  logepoch_.debugStream() << "H5Epoch starting session (somatime: "
			<< ts << ", walltime: " << time << ")";  
  
}
void H5Epoch::endSession(somatime_t ts, int time, const std::map<dpair_t, SinkStats> & stats )
{
  /*
    Actually end the session, update session data, etc. 

  */
  currentSession_.endts = ts; 
  currentSession_.endtime = time; 
  
  // save session
  sessionCache_.push_back(currentSession_); 
  
  // save as attribute
  logepoch_.debugStream() << "H5Epoch ending session (from somatime: "
			<< currentSession_.startts << ", walltime: " 
			<< currentSession_.starttime << " TO somatime: " 
			<< currentSession_.endts << " walltime: " 
			<< currentSession_.endtime << " ) "; 

  saveSessionAttribute(h5group_, sessionCache_); 
  
  currentSession_.startts = 0; 
  currentSession_.starttime = 0; 
  currentSession_.endts = 0; 
  currentSession_.endtime = 0; 


  // flush the data
  for (dispatchTable_t::iterator psink= dispatchTable_.begin(); 
       psink != dispatchTable_.end(); psink++) {
    (*(psink->second))->flush(); 
    // now save the session stats
    std::map<dpair_t, SinkStats>::const_iterator i = stats.find(psink->first); 
    
    if (i == stats.end()) {
      logepoch_.error("datasink dpair_t not found in argument stats"); 
      
    } else {

      (*(psink->second))->appendStats(i->second); 
    }
  }

}

std::set<dpair_t> H5Epoch::getDataSinks() {
  
  std::set<dpair_t> results; 
  for (dispatchTable_t::iterator d = dispatchTable_.begin(); 
       d != dispatchTable_.end(); d++) {
    results.insert(d->first); 
    
  }
  return results; 
  
}

void H5Epoch::enableDataSink(datasource_t src, datatype_t typ)
{
  /*
    The enable/disable methods only work if you have never recorded
    into the epoch. 

  */
  logepoch_.debug("H5Epoch::enableDataSink(datasource_t src, datatype_t typ)"); 

  // check if we're recording, and if so throw an exception
  if(isRecording_ or hasRecorded_) {
    logepoch_.warn("attempted to modify data sinks while /after recording"); 
    throw std::runtime_error("Cannot modify data sinks when recording"); 
  }
  dpair_t tgtpair = make_pair(src, typ); 
  // first, check if already enabled, and if so, don't do anything
  if (dispatchTable_.find(tgtpair) != dispatchTable_.end()) {
    logepoch_.warnStream() << "Enabled already-enabled data sink " 
			   << "(" << (int)src << ", " << (int)typ << ")"; 

    return; 
  }
  
  if (sourceToName_.find(src) == sourceToName_.end()) {
    // create placeholder name
    boost::format namefmt("source%2.2d"); 
    namefmt % (int)src; 
    sourceToName_[src] = str(namefmt); 
  }
  
  std::string name = sourceToName_[src]; 

  if (typ == TSPIKE) {
    if (!haveTSpikeGroup_) {
      logepoch_.debug("enableDataSink::tspike group not found; creating"); 
      h5TSpikeGroup_ = h5group_.createGroup("TSpike"); 
      haveTSpikeGroup_ = true; 
    }

    assert(haveTSpikeGroup_); 
    logepoch_.debugStream() << "enableDataSink::creating TSpikeTable" 
			    << "name ='" << name << "' src =" << src; 

    
    pTSpikeTable_t tstable = TSpikeTable::create(h5TSpikeGroup_, name, src); 
    
    dispatchTable_[make_pair(src, TSPIKE)] = tstable; 
    sourceToSink_.insert(std::make_pair(src, pDatasetIO_t(tstable))); 

  } else if (typ == WAVE) {
    if (!haveWaveGroup_) {
      logepoch_.debug("enableDataSink::wave group not found; creating"); 
      h5WaveGroup_ = h5group_.createGroup("Wave"); 
      haveWaveGroup_ = true; 
    }
    assert(haveWaveGroup_); 
    logepoch_.debugStream() << "enableDataSink::creating WaveTable" 
			    << "name ='" << name << "' src =" << src; 

    pWaveTable_t wtable = WaveTable::create(h5WaveGroup_, name, src); 
    
    dispatchTable_[make_pair(src, WAVE)] = wtable; 
    sourceToSink_.insert(std::make_pair(src, pDatasetIO_t(wtable))); 
  }
  
}

void H5Epoch::disableDataSink(datasource_t src, datatype_t typ)
{
  // NOTE THIS MIGHT POTENTIALLY DELETE DATA?
  // check if we're recording, and if so throw an exception
  // FIXME
  if(isRecording_ or hasRecorded_) {
    logepoch_.warn("attempted to modify data sinks while /after recording"); 

    throw std::runtime_error("Cannot modify data sinks when recording"); 
  }  
  // first remove the object; this should cause deletion
  // of the object 
  std::string path = dispatchTable_[make_pair(src, typ)]->getPath(); 
  
  h5TSpikeGroup_.unlink(path); 
  dispatchTable_.erase(make_pair(src, typ)); 
  // now delete the actual object in the file

  // Finally, check if the group is empty, if so, delete
  if (typ == TSPIKE) {
    if (h5TSpikeGroup_.getNumObjs() == 0) {
      logepoch_.infoStream() << "All TSpike data sources from " << getName() 
			      << " deleted, removing group"; 

	
      h5group_.unlink("/" + getName() + "/" + "TSpike"); 
      h5TSpikeGroup_.close(); 
      haveTSpikeGroup_ = false; 
    }
    
  } else if (typ == WAVE) {
    if (!h5WaveGroup_.getNumObjs() == 0 ) {
      logepoch_.infoStream() << "All Wave data sources from " << getName() 
			      << " deleted, removing group"; 

      h5group_.unlink("/" + getName() + "/" + "Wave"); 
      h5WaveGroup_.close(); 
      haveWaveGroup_ = false; 
    }
    
  }
  
}

void H5Epoch::setDataName(datasource_t src, std::string name)
{
  logepoch_.debug("H5Epoch::setDataName(datasource_t src, std::string name"); 

  if(isRecording_ or hasRecorded_) {
    logepoch_.warn("attempted to rename data sinks while /after recording"); 
    throw std::runtime_error("Cannot modify data sinks name when recording"); 
  }  
  sourceToName_[src] = name; 
  sourceSinkTable_t::iterator keyiter, keyend;
  keyiter = sourceToSink_.find(src); 
  keyend = sourceToSink_.upper_bound(src); 
  for (; keyiter != keyend; keyiter++) {

    // get the current path + the name
    std::string datasetpath = keyiter->second->getPath(); 
    
    std::string containingpath = h5helper::extractBase(datasetpath); 
    
    // get the new path + the name
    std::string tgt =  containingpath + "/" + name;

    h5group_.move(datasetpath, tgt); 
    keyiter->second->updateName(); 

    logepoch_.infoStream() << "moving data sink from "  << datasetpath
			   << " to " << tgt; 

  }

}

std::string H5Epoch::getDataName(datasource_t src)
{
   if (sourceToName_.find(src) == sourceToName_.end()){
     return ""; 
   } else {
     return sourceToName_[src]; 
   }
}


void H5Epoch::appendData(const pDataPacket_t pkt)
{
  dpair_t tgtdpair(pkt->src, pkt->typ); 
  logepoch_.infoStream() << "Received packet with sequence id= " 
			 << pkt->seq; 
  // FIXME: what do we do if we get an incorrect packet? 

  // This is ugly, but is because the [] operator in map
  // could potentially try and create a new instance of the class
  // which in our class is an ABC
  if (dispatchTable_.find(tgtdpair) != dispatchTable_.end()) {
    (*(dispatchTable_.find(tgtdpair)->second))->append(pkt); 
  } else {
    logepoch_.warnStream() << "Received unexpected data packet for" 
			   << " src = " << (int)pkt->src
			   << " type =" << (int)pkt->typ; 
  }

}


// pNoteTable_t H5Epoch::openNoteTable()
// {
//   // either open or create the note table for this epoch
//   // FIXME! Open? maybe? 
//   pNoteTable_t nt(new NoteTable(h5group_)); 
  
//   return nt; 
		   
// }

void H5Epoch::appendEvent(pEventPacket_t pel)
{
  // First, check against our current event mask
  EventList_t::iterator ei; 
  ei=pel->events->begin(); 
  for(; ei != pel->events->end(); ++ei) {
    if(eventMaskSet_.isSet(ei->src, ei->cmd)) {
      // turn it into a seqevent
      SeqEvent_t se; 
      se.seq = pel->seq; 
      se.cmd = ei->cmd; 
      se.src = ei->src; 
      se.data = ei->data; 
      pEventTable_->append(se); 

    } 
  }
  
}


std::map<std::string, std::string> H5Epoch::getSessionStatistics(int session)
{
  std::map<std::string, std::string> results; 
  
  results["totaldata"] = "12345678"; 
  
  return results; 
}

std::vector<SinkStats> H5Epoch::getSinkStatistics(datasource_t src, datatype_t typ)
{
  /* Returns a vector of statistics for this sink, one per session. 
     
     Are these only updated infrequently? on the exit of a session? 

     How do they integrate with file opening? 
     
     What do we actually save on-disk? 

   */ 
  
  dpair_t d = std::make_pair(src, typ); 
  if (dispatchTable_.find(d) == dispatchTable_.end()) {
    throw std::runtime_error("src/typ pair not enabled"); 
  }
  // we assume it's in the cache? 

  return (*(dispatchTable_.find(d)->second))->getStats();
  
}

void H5Epoch::addEventRXMask(eventsource_t src, const std::vector<eventcmd_t> & cmds) 
{
 
  pEventTable_->addEventRXMask(src, cmds); 
 
}

std::vector<eventcmd_t> H5Epoch::getEventRXMask(eventsource_t src) {
  
  return pEventTable_->getEventRXMask(src); 

}

void H5Epoch::removeEventRXMask(eventsource_t src, const std::vector<eventcmd_t> & cmds)
{

  pEventTable_->removeEventRXMask(src, cmds); 

}

void H5Epoch::saveSessionAttribute(H5::Group node, const std::vector<Session>& sessions)
{

  // create session
  H5::CompType sessionType(sizeof(Session)); 
  sessionType.insertMember("startts", HOFFSET(Session, startts), H5::PredType::NATIVE_UINT64); 
  sessionType.insertMember("starttime", HOFFSET(Session, starttime), H5::PredType::NATIVE_INT32); 
  sessionType.insertMember("endts", HOFFSET(Session, endts), H5::PredType::NATIVE_UINT64); 
  sessionType.insertMember("endtime", HOFFSET(Session, endtime), H5::PredType::NATIVE_INT32); 
  
  const int LENGTH = sessions.size(); 
  const int RANK = 1; 
  hsize_t dim[] = {LENGTH};   /* Dataspace dimensions */
  H5::DataSpace space( RANK, dim );

  H5::Attribute sessionattribute; 
  bool exists = false; 
  
  for (int i = 0; i < node.getNumAttrs();  ++i) {
   H5::Attribute a = node.openAttribute(i); 
    if (a.getName() == "sessions") {
      exists = true; 
      sessionattribute=a; 
    }

  }
  if (!exists) {
    sessionattribute = 
      node.createAttribute("sessions", sessionType, space);
  }
  
  sessionattribute.write(sessionType, &sessions[0]); 
  sessionattribute.close();
}

std::vector<Session> H5Epoch::loadSessionAttribute(H5::Group node)
{
  H5::CompType sessionType(sizeof(Session)); 
  sessionType.insertMember("startts", HOFFSET(Session, startts), H5::PredType::NATIVE_UINT64); 
  sessionType.insertMember("starttime", HOFFSET(Session, starttime), H5::PredType::NATIVE_INT32); 
  sessionType.insertMember("endts", HOFFSET(Session, endts), H5::PredType::NATIVE_UINT64); 
  sessionType.insertMember("endtime", HOFFSET(Session, endtime), H5::PredType::NATIVE_INT32); 
  
  bool found = false; 
  for (int i = 0; i < node.getNumAttrs(); i++) {
    H5::Attribute a = node.openAttribute(i); 
    if (a.getName() == std::string("sessions")) {
      found = true; 
    }
    
  }

  if (!found) {
    std::vector<Session> sessions; 
    return sessions; 
  }

  H5::Attribute a = node.openAttribute("sessions"); 
  
  H5::DataSpace space = a.getSpace(); 
  

  hsize_t dims[1]; 
  hsize_t maxdims = 1; 
  space.getSimpleExtentDims(dims, &maxdims); 

  std::vector<Session> sessions(dims[0]); // allocate the correct number

  a.read(sessionType, &sessions[0]); 
  a.close();
  return sessions; 
  
}

void H5Epoch::setCurrentTS(somatime_t ts)
{
  if (isRecording_) {
    currentSession_.endts = ts; 
  }


}

notehandle_t H5Epoch::createNote() 
{
  return pNoteTable_->createNote(); 
}

Note_t H5Epoch::getNote(notehandle_t nid)
{
  return pNoteTable_->getNote(nid); 

}

std::list<notehandle_t> H5Epoch::getNotes()
{
  return pNoteTable_->getNotes(); 

}

void H5Epoch::setNote(const Note_t & note)
{
  pNoteTable_->setNote(note); 
}

void H5Epoch::deleteNote(notehandle_t nid)
{
  pNoteTable_->deleteNote(nid); 

}

