#ifndef DATASETIO_H
#define DATASETIO_H


#include <boost/array.hpp>
#include <somanetwork/datapacket.h>
#include <somanetwork/event.h>
#include <boost/shared_ptr.hpp>


class DatasetIO
{
 public:
  virtual void append(const pDataPacket_t) = 0;
  virtual ~DatasetIO() 
    {}; 
};


#endif //DATASETIO_H
