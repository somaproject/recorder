#ifndef DATASETIO_H
#define DATASETIO_H

#include <boost/array.hpp>

struct RawData
{
  int seq; 
  unsigned char src; 
  unsigned char typ; 
  bool missing; 
  boost::array<char, 580> body;
}; 


class DatasetIO
{
 public:
  virtual void append(const RawData *) = 0;
  
};


#endif //DATASETIO_H
