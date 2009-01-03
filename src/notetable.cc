#include <boost/format.hpp> 
#include <iostream>
#include <somanetwork/wave.h>
#include <somanetwork/datapacket.h>

#include "notetable.h"

using namespace soma::recorder; 

int min(int x, int y) {
  if (x < y)
    return x;
  return y;
}
pNoteTable_t NoteTable::create(H5::Group containingGroup) 
{
  pNoteTable_t pnt(new NoteTable(containingGroup));
  
  size_t Note_dst_size =  sizeof( NoteDisk_t );
    
  herr_t     status;
  const char * Note_field_names[NFIELDS] = 
    { "handle", "createtime", "createts", "modifytime", "modifyts",
    "name", "text", "tags"}; 

  
  hid_t Note_field_type[NFIELDS]; 
  Note_field_type[0] = H5T_NATIVE_UINT32; 
  Note_field_type[1] = H5T_NATIVE_UINT64; 
  Note_field_type[2] = H5T_NATIVE_UINT64; 
  Note_field_type[3] = H5T_NATIVE_UINT64; 
  Note_field_type[4] = H5T_NATIVE_UINT64; 

  hid_t name_string_type = H5Tcopy( H5T_C_S1 );
  H5Tset_size( name_string_type, NAMELEN );
  Note_field_type[5] = name_string_type; 

  hid_t text_string_type = H5Tcopy( H5T_C_S1 );
  H5Tset_size( text_string_type, NOTELEN );
  Note_field_type[6] = text_string_type; 

  hid_t tag_string_type = H5Tcopy( H5T_C_S1 );
  H5Tset_size( tag_string_type, TAGLEN );
  Note_field_type[7] = tag_string_type; 

  
  status = H5TBmake_table( "Notes Table", 
			   pnt->tableLoc_.getLocId(),
			   pnt->tableName_.c_str(), 
			   NFIELDS, 
			   0, 
			   Note_dst_size, 
			   Note_field_names, 
			   &(pnt->dstOffsets_[0]), 
			   Note_field_type,
			   100, NULL, 
			   false, NULL  );
  
  pnt->selfDataSet_ = pnt->tableLoc_.openDataSet(pnt->tableName_); 
  return pnt; 

}

 pNoteTable_t NoteTable::open(H5::Group containingGroup)
{
  pNoteTable_t pnt(new NoteTable(containingGroup));
  
  pnt->selfDataSet_ = pnt->tableLoc_.openDataSet(pnt->tableName_); 
  
  // load the relevant data
  throw std::runtime_error("Not implemented"); 
  
}


NoteTable::NoteTable(H5::Group gloc) :
  tableLoc_(gloc), 
  latestNoteHandle_(0)
{
  // construct the table
  tableName_ = "notes"; 

  setupDataTypes(); 
  
}

void NoteTable::setupDataTypes()
{

  
  dstOffsets_.push_back(HOFFSET( NoteDisk_t, handle)); 
  dstOffsets_.push_back(HOFFSET( NoteDisk_t, createtime)); 
  dstOffsets_.push_back(HOFFSET( NoteDisk_t, createts)); 
  dstOffsets_.push_back(HOFFSET( NoteDisk_t, modifytime)); 
  dstOffsets_.push_back(HOFFSET( NoteDisk_t, modifyts)); 
  dstOffsets_.push_back(HOFFSET( NoteDisk_t, name)); 
  dstOffsets_.push_back(HOFFSET( NoteDisk_t, text)); 
  dstOffsets_.push_back(HOFFSET( NoteDisk_t, tags)); 



  NoteDisk_t tests; 
  dstSizes_.push_back(sizeof( tests.handle)); 
  dstSizes_.push_back(sizeof( tests.createtime)); 
  dstSizes_.push_back(sizeof( tests.createts)); 
  dstSizes_.push_back(sizeof( tests.modifytime)); 
  dstSizes_.push_back(sizeof( tests.modifyts)); 
  dstSizes_.push_back(sizeof( tests.name)); 
  dstSizes_.push_back(sizeof( tests.text)); 
  dstSizes_.push_back(sizeof( tests.tags)); 

}

notehandle_t NoteTable::createNote()
{

  Note_t note; 
  note.createts = 0; 
  note.createtime = 0; 
  note.modifyts = 0; 
  note.modifytime = 0; 
  
  latestNoteHandle_++; 
  
  // note.createts ==  // FIXME: SET CURRENT TIMES 
  note.handle = latestNoteHandle_; 
  noteMap_[latestNoteHandle_] = note; 
  return latestNoteHandle_; 

}

Note_t NoteTable::getNote(notehandle_t hid)
{
  notemap_t::iterator pn = noteMap_.find(hid); 
  if (pn == noteMap_.end() ) {
    throw std::runtime_error("Note not found"); // FIXME
  }
  
  return (pn->second); 
}

void NoteTable::setNote(notehandle_t handle, std::string name, std::string text, 
			std::vector<std::string> tags)
{

  // check string lengths
  if (name.length() > NAMELEN) {
    throw std::runtime_error("Note name is too long"); // FIXME
  } 

  if (text.length() > NOTELEN) {
    throw std::runtime_error("Note text is too long"); // FIXME
  } 


  noteMap_[handle].text = text; 
  noteMap_[handle].name = name; 
  // a little bit of format marshalling -- there has GOT to be a better
  // way of doing this FIXME need to implement

}

void NoteTable::setNote(const Note_t & note)
{

  // check string lengths
  if (note.name.length() > NAMELEN) {
    throw std::runtime_error("Note name is too long"); // FIXME
  } 

  if (note.text.length() > NOTELEN) {
    throw std::runtime_error("Note text is too long"); // FIXME
  } 


  noteMap_[note.handle] = note; 
  // a little bit of format marshalling -- there has GOT to be a better
  // way of doing this FIXME need to implement

}

void NoteTable::deleteNote(notehandle_t hid)
{
  noteMap_.erase(hid); 
}

std::list<notehandle_t> NoteTable::getNotes()
{
  std::list<notehandle_t> notes; 
  for (notemap_t::iterator pn = noteMap_.begin(); 
       pn != noteMap_.end(); pn++) {
    notes.push_back(pn->second.handle); 
  }
  return notes; 
 
}

void NoteTable::flush()
{
  // save the current note map to disk by clearing the existing one, and
  // saving ours
  
  // first create a vector of disk tables
  std::vector<NoteDisk_t> diskNotes;
  diskNotes.reserve(noteMap_.size()); 

  for (notemap_t::iterator pn = noteMap_.begin(); 
       pn != noteMap_.end(); pn++) {
    Note_t n = pn->second; 
    NoteDisk_t nd; 
    nd.createtime = n.createtime; 
    nd.createts = n.createts; 
    nd.modifytime = n.modifytime; 
    nd.modifyts = n.modifyts; 
    
    // copy the strings 
    bzero(&nd.name[0], NAMELEN); 
    n.name.copy(&nd.name[0], NAMELEN-1); 
    nd.name[min(NAMELEN-1, n.name.size())] = 0; // append termination; 

    bzero(&nd.text[0], NOTELEN); 
    n.text.copy(&nd.text[0], NOTELEN-1); 
    nd.text[min(NOTELEN-1, n.text.size())] = 0; // append termination; 
    
    diskNotes.push_back(nd); 

  }
  if (diskNotes.size() > 0) {
    H5TBappend_records(tableLoc_.getLocId(), tableName_.c_str(), 
		       diskNotes.size(), 
		       sizeof(NoteDisk_t), 
		       &dstOffsets_[0], 
		       &dstSizes_[0], 
		       &diskNotes[0]); 
  }
  
}

NoteTable::~NoteTable()
{
  flush(); 

}

void NoteTable::setCurrentTS(somatime_t ts)
{
  currentTS_ = ts; 
}

void NoteTable::setCurrentTime(int time)
{
  currentTime_ = time; 
}
