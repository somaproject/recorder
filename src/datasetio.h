#ifndef DATASETIO_H
#define DATASETIO_H

#include <boost/array.hpp>
#include <netdata/rawdata.h>

class DatasetIO
{
 public:
  virtual void append(const RawData *) = 0;
  
};


#endif //DATASETIO_H
