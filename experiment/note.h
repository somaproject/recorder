#ifndef NOTES_H
#define NOTES_H

#include <boost/array.hpp>

namespace soma {
  namespace recorder {
    typedef int notehandle_t;   // per-table note handle
    
    struct Note_t {
      notehandle_t handle; 
      // name
      std::string name; 
      // create time
      uint64_t createtime; 
      uint64_t createts; 
  
      uint64_t modifytime; 
      uint64_t modifyts; 
      
      // actual text
      std::string text; 
      std::vector<std::string> tags; 

    }; 
  }
}


#endif // NOTES_H
