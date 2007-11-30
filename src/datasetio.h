#ifndef DATASETIO_H
#define DATASETIO_H

#include <boost/array.hpp>
#include <somanetwork/datapacket.h>
#include <somanetwork/event.h>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace soma {
  namespace recorder {
    
    class DatasetIO : boost::noncopyable
    {
    public:
      virtual void append(pDataPacket_t) = 0;
      virtual ~DatasetIO() 
      {}; 
    };
    
    typedef boost::shared_ptr<DatasetIO> pDatasetIO_t; 
  }
}

#endif //DATASETIO_H
