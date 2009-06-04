#include <boost/filesystem/operations.hpp>
#include "dbusexperiment.h"
#include "dbusepoch.h"
#include "h5experiment.h"
#include <somanetwork/network.h>
#include <glibmm.h>

using namespace soma::recorder; 


DBUSExperiment::DBUSExperiment(DBus::Connection & connection, 
			       Glib::RefPtr<Glib::MainLoop> ml, 
			       std::string filename, 
			       pNetworkInterface_t network, 
			       bool create=true) 
  : DBus::ObjectAdaptor(connection, "/soma/recording/experiment"), 
    pNetwork_(network), 
    glibmainloop_(ml), 
    connection_(connection), 
    nextEpochDBusID_(0),
    logdbus_(log4cpp::Category::getInstance("soma.recording.experiment.dbus"))

{
  boost::filesystem::path filepath(filename); 

  if (create) {
    pExperiment_ = H5Experiment::create(pNetwork_, filepath); 
  } else {
    pExperiment_ = H5Experiment::open(pNetwork_, filepath); 
  }
  
  // now get all of the epochs that might exist with the file, and
  // expose them with correct object IDs
  std::vector<pEpoch_t> epochs = pExperiment_->getEpochs(); 
  for(std::vector<pEpoch_t>::iterator ei = epochs.begin(); 
      ei != epochs.end(); ei++) {
    DBUSEpoch * dbe = new DBUSEpoch(connection_, *ei, nextEpochDBusID_); 
    nextEpochDBusID_++; 
    dbusEpochs_.push_back(dbe); 
    EpochCreate(dbe->path()); 
  }
  
}


std::map< std::string, std::string > DBUSExperiment::GetFileProperties()
{
  std::map< std::string, std::string > result; 
  // FIXME: Add in real properties
  result["Foo"] = "Bar"; 
  return result; 
}



std::vector< DBus::Path > DBUSExperiment::GetEpochs(  )
{
  logdbus_.debug("GetEpochs called"); 

  std::vector< DBus::Path > result; 
  for (std::vector<DBUSEpoch *>::iterator pdbe = dbusEpochs_.begin(); 
       pdbe != dbusEpochs_.end(); pdbe++) {
    result.push_back((*pdbe)->path()); 
  }
  return result; 
}


DBus::Path DBUSExperiment::CreateEpoch( const std::string& name )
{
  logdbus_.debug("CreateEpoch called"); 

  try {
    pEpoch_t epoch = pExperiment_->createEpoch(name); 
    
    DBUSEpoch * dbe = new DBUSEpoch(connection_, epoch, nextEpochDBusID_); 
    nextEpochDBusID_++; 

    dbusEpochs_.push_back(dbe); 
    
    EpochCreate(dbe->path()); 
    logdbus_.infoStream() << "Created epoch with name " 
			  << name;
    return dbe->path();   
 
  } catch (exception & e) {
    logdbus_.warnStream() << "Epoch name " << name << "already exists (" << e.what() << ")";  
    throw DBus::Error("soma.Experiment", "epoch named %s already exists" ); 
  }

}

DBus::Path DBUSExperiment::GetEpoch( const std::string& name )
{
  logdbus_.debug("GetEpoch called"); 

  pEpoch_t epoch = pExperiment_->getEpoch(name); 
  
  // FIXME: this is inefficient
  for(int i = 0; i < dbusEpochs_.size(); i++){
    if (dbusEpochs_[i]->getEpochPointer() == epoch) {
      return dbusEpochs_[i]->path(); 
    }
  }

}

void DBUSExperiment::Close()
{
  logdbus_.debug("Close called"); 
  
  try {
    // first, try and close all of the epochs
    std::vector<DBUSEpoch *>::iterator idepoch; 
    for(idepoch = dbusEpochs_.begin(); idepoch != dbusEpochs_.end(); idepoch++){
      delete (*idepoch); 
    }

    pExperiment_->close(); 
    glibmainloop_->quit(); 
  } catch (Exception & e) {
    
    throw DBus::Error("soma.Experiment", "Cannot close experiment"); 	
  }
  
}

std::string  DBUSExperiment::GetName()
{
  return pExperiment_->getName(); 
}

void DBUSExperiment::RenameEpoch( const ::DBus::Path& epoch, const
				  std::string& newname )
{
  // FIXME : this is an ugly hack, and I'm now less convinced
  // that the EpochRenaming needs to be an experiment method
  // as opposed to an epoch method. Oh well. 


  std::vector<DBUSEpoch *>::iterator dbei; 
  for(dbei= dbusEpochs_.begin(); dbei != dbusEpochs_.end(); dbei++){
    if((*dbei)->path() == epoch) {
      pExperiment_->renameEpoch((*dbei)->getEpochPointer(), newname); 
      break; 
    }
  }

}


// notes work

int32_t DBUSExperiment::CreateNote()
{
  return pExperiment_->createNote(); 
}

void DBUSExperiment::GetNote( const int32_t& notehandle, std::string& name, uint64_t& createtime, uint64_t& createts, uint64_t& edittime, uint64_t& editts, std::vector< std::string >& tags )
{
  Note_t n = pExperiment_->getNote(notehandle); 
  name = n.name; 
  createtime = n.createtime; 
  createts = n.createts; 
  edittime = n.modifytime; 
  editts = n.modifyts; 
  tags = n.tags; 
  
}

void DBUSExperiment::SetNote( const int32_t& notehandle, const std::string& name, 
			      const std::string& notetext, const std::vector< std::string >& tags )
{
  Note_t note; 
  note.handle = notehandle; 
  note.name = name; 
  note.text = notetext; 
  

  for(std::vector< std::string >::const_iterator i = tags.begin(); 
      i != tags.end(); i++) {
    note.tags.push_back(*i); 
  }
  
  pExperiment_->setNote(note); 

}

void DBUSExperiment::DeleteNote(const int32_t& nodehandle)
{
  pExperiment_->deleteNote(nodehandle); 
}

std::vector< int32_t > DBUSExperiment::GetNotes()
{
  std::vector<int32_t> results; 
  std::list<notehandle_t> notes = pExperiment_->getNotes(); 
  for(std::list<notehandle_t>::iterator i =  notes.begin(); i != notes.end(); i++) {
    results.push_back(*i); 
  }
  return results; 

}

uint64_t DBUSExperiment::GetReferenceTime() {
  return pExperiment_->getReferenceTime(); 
}

void DBUSExperiment::MarkReferenceTime() {
  // we need to get the time from the h5file and then set the time? 
  somatime_t ts = pExperiment_->getCurrentTS(); 
  pExperiment_->setReferenceTime(ts); 
  ReferenceTimeChange(ts); 

}
