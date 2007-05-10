#ifndef H5FILERECORDER_H
#define H5FILERECORDER_H


#include <string>
#include <H5Cpp.h>

enum DATATYPES {TSPIKE, WAVE, RAW};

namespace soma 
{ 
  namespace recorder {
 

class H5FileRecorder
  { 
  public: 
    H5FileRecorder(const std::string & filename); 
    ~H5FileRecorder(); 
    void  createEpoch(const std::string & epochName); 
    void switchEpoch(const std::string & epochName); 
    void enableRX(DATATYPES typ, int src); 

  private:
    std::string filename_; 
    H5::H5File h5file_; 
    H5::Group epochGroup_;

    H5::Group getTypeGroup(DATATYPES typ);

    friend void H5FileRecorder_test(); 

  };
  
  }
}

#endif // H5FILERECORDER_H
