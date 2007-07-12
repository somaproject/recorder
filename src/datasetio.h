#ifndef DATASETIO_H
#define DATASETIO_H

#include <boost/array.hpp>
#include <somanetwork/datapacket.h>

class DatasetIO
{
 public:
  virtual void append(const DataPacket_t *) = 0;
  virtual ~DatasetIO() 
    {}; 
};

//DatasetIO::~DatasetIO() {}

#endif //DATASETIO_H
