#include <boost/format.hpp>

#include "h5epoch.h"
#include "tspiketable.h"
#include "wavetable.h"


using namespace soma::recorder; 

H5Epoch::H5Epoch(H5::Group g, epochname_t name) :
  h5group_(g), 
  name_(name)
{

  pEventTable_ = openEventTable(); 
  pNoteTable_ = openNoteTable(); 

}

H5Epoch::~H5Epoch()
{


}


epochname_t H5Epoch::getName() 
{
  return name_; 
}

void H5Epoch::enableDataSink(datasource_t src, 
			      datatype_t typ, std::string name)
{
  // Things to verify: 
  //  the src/type pair is not already in use
  //  within this type the name is not already in use

  // check if this pair is already in use:
  if (dispatchTable_.find(dpair_t(src, typ)) != dispatchTable_.end() ) 
    {
      throw std::runtime_error("Source/type pair in use"); 
    }

  // Check if the name is already in use in this type
  for (namedSinkTable_t::iterator ns = namedSinkTable_.begin(); 
       ns != namedSinkTable_.end(); ++ns ) {
    if (ns->first.second == typ and ns->second == name) {
      boost::format errormsg("%s is already assigned to source %d of this type"); 
      throw std::runtime_error(boost::str(errormsg % name % src)); 

    }
  }
  
  dpair_t tgtdpair(src, typ); 

  // enable
  namedSinkTable_[tgtdpair] = name; 

  // get file group
  H5::Group group = getTypeGroup(typ); 
  
  // setup 
  switch(typ) 
    {
    case TSPIKE:
      dispatchTable_.insert(tgtdpair, 
			    new TSpikeTable(src, name, group)); 
      break; 
    case WAVE:
      dispatchTable_.insert(tgtdpair, 
			    new WaveTable(src, name, group)); 
      break; 
    case RAW:
      // FIXME : not implemented yet
      break; 
    }

}

H5::Group H5Epoch::getTypeGroup(datatype_t typ)
{
  // get the group for the type if it exists
  std::string typeName;
  switch(typ) 
    {
    case TSPIKE:
      typeName = "tspikes"; 
      break; 
    case WAVE:
      typeName = "waves"; 
      break; 
    case RAW:
      typeName = "raw"; 
      break; 

    }

  H5::Group hg; 

  try 
    {
      hg =  h5group_.openGroup(typeName); 
    } 
  
  catch(H5::GroupIException & e){ 
    std::cout << "Creating group ..." << typeName << std::endl; 
    hg =  h5group_.createGroup(typeName); 
  }
  
  return hg; 
}

void H5Epoch::disableDataSink(datasource_t src, datatype_t typ)
{

  dpair_t tgtdpair(src, typ); 

  
  if (dispatchTable_.find(dpair_t(src, typ)) == dispatchTable_.end() ) 
    {
      throw std::runtime_error("Source/type pair is not enabled"); 
    }

  namedSinkTable_.erase(tgtdpair); 
  dispatchTable_.erase(tgtdpair); 

}

std::string H5Epoch::getDataSinkName(datasource_t src, datatype_t typ)
{
  
  dpair_t tgtdpair(src, typ); 
  return namedSinkTable_[tgtdpair]; 

}

std::list<dpair_t> H5Epoch::getAllDataSinks(){
  dispatchTable_t::iterator i; 
  std::list<dpair_t> outlist; 
  for (i = dispatchTable_.begin(); i != dispatchTable_.end(); i++)
    {
      outlist.push_back(i->first); 
    }

  return outlist; 

}

void H5Epoch::appendData(const pDataPacket_t pkt)
{
  dpair_t tgtdpair(pkt->src, pkt->typ); 
  // FIXME: what do we do if we get an incorrect packet? 

  // This is ugly, but is because the [] operator in map
  // could potentially try and create a new instance of the class
  // which in our class is an ABC
  dispatchTable_.find(tgtdpair)->second->append(pkt); 
  
}

pEventTable_t H5Epoch::openEventTable()
{
  // either open or create the event table for this epoch
  pEventTable_t et(new EventTable(h5group_)); 
  
  return et; 
		   
}

pNoteTable_t H5Epoch::openNoteTable()
{
  // either open or create the note table for this epoch
  // FIXME! Open? maybe? 
  pNoteTable_t nt(new NoteTable(h5group_)); 
  
  return nt; 
		   
}

void H5Epoch::appendEvent(const pEventPacket_t pep)
{
  pEventTable_->append(pep); 

}

void H5Epoch::createNote(std::string name, std::string text)
{
  pNoteTable_->createNote(name, text); 

}

void H5Epoch::setNote(std::string name, std::string text)
{
  pNoteTable_->setNote(name, text); 

}

Note_t H5Epoch::getNote(std::string name)
{
  return pNoteTable_->getNote(name); 

}
 
void H5Epoch::deleteNote(std::string name)
{

  pNoteTable_->deleteNote(name); 

}
  
std::list<Note_t> H5Epoch::getAllNotes()
{

  return pNoteTable_->getAllNotes(); 

}
