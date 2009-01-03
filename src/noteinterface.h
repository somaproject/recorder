#ifndef NOTEINTERFACE_H
#define NOTEINTERFACE_H

#include <list>

#include "note.h"
namespace soma {
  namespace recorder {
    class NoteInterface {
    public:

      // note manipulation
      virtual notehandle_t createNote() = 0;  
      virtual Note_t getNote(notehandle_t) = 0; 
      virtual std::list<notehandle_t> getNotes() = 0; 
      virtual void setNote(const Note_t & note) = 0; 
      virtual void deleteNote(notehandle_t) = 0; 
      

    }; 

  }
}

#endif // NOTEINTERFACE_H
