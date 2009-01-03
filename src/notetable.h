#ifndef NOTETABLE_H
#define NOTETABLE_H

#include <vector>
#include <list>
#include <map>

#include "H5Cpp.h"
#include "hdf5_hl.h"
#include "note.h" 

#include "datasetio.h"

using namespace H5;
using namespace soma::recorder; 

namespace soma {
  namespace recorder {
    typedef uint64_t somatime_t; 
    typedef std::map<notehandle_t, Note_t> notemap_t; 

    const int NAMELEN = 50; 
    const int NOTELEN = 2048; 
    const int TAGLEN = 100; 

    struct NoteDisk_t {
      // representation of the note on disk for 
      // serialization
      notehandle_t handle; 
      boost::array<char, NAMELEN> name; 
      uint64_t createtime; 
      uint64_t createts; 
      uint64_t modifytime; 
      uint64_t modifyts; 
      boost::array<char, NOTELEN> text; 
      boost::array<char, TAGLEN> tags; 
    }; 

    class NoteTable; 
    typedef boost::shared_ptr<NoteTable> pNoteTable_t; 

    class NoteTable 
      {
	
      public:
	static pNoteTable_t create(H5::Group containingGroup); 

	static pNoteTable_t open(H5::Group containingGroup); 

	~NoteTable(); 
	
	notehandle_t createNote(); 
	Note_t getNote(notehandle_t);  
	void setNote(notehandle_t handle, std::string name, std::string notetxt, 
		     std::vector<std::string> tags); 
	void setNote(const Note_t & note);
	void deleteNote(notehandle_t); 
	std::list<notehandle_t> getNotes(); 
	
	void setCurrentTS(somatime_t); 
	void setCurrentTime(int); 


	void flush();
	
      private:
	NoteTable(H5::Group gloc); 


	void setupDataTypes(); 

	void createTable(); 

	H5::Group tableLoc_; 
	std::vector<size_t> dstSizes_; 
	std::vector<size_t> dstOffsets_; 
	std::string tableName_; 
	notemap_t noteMap_; 

	static const int NFIELDS = 8; 
	somatime_t currentTS_; 
	int currentTime_; 
	void loadFromDisk();

	H5::DataSet selfDataSet_; 
	
	notehandle_t latestNoteHandle_; 
      }; 
    typedef boost::shared_ptr<NoteTable> pNoteTable_t; 

  }
}


#endif // NOTETABLE_H
