#ifndef NOTES_H
#define NOTES_H

#include <boost/array.hpp>

namespace soma {
  namespace recorder {

    struct Note_t {
      // name
      std::string name; 
      // create time
      uint64_t createtime; 
      uint64_t createts; 
  
      uint64_t modifytime; 
      uint64_t modifyts; 
      
      // actual text
      std::string text; 
      
    }; 
  }
}


#endif // NOTES_H
