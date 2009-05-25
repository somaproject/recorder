#ifndef DATASETIO_H
#define DATASETIO_H

#include <boost/array.hpp>
#include <somanetwork/datapacket.h>
#include <somanetwork/event.h>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include "sinkstats.h" 


namespace soma {
  namespace recorder {
    using namespace somanetwork; 

    class DatasetIO : boost::noncopyable
    {
    public:
      virtual void append(const pDataPacket_t) = 0;
      virtual void updateName() = 0; 
      virtual std::string getPath() = 0; 
      virtual void flush() = 0;
      
      virtual void appendStats(const SinkStats & stats) = 0; 
      virtual std::vector<SinkStats> getStats() = 0; 
      virtual ~DatasetIO() {}; 
      

    };
    
    typedef boost::shared_ptr<DatasetIO> pDatasetIO_t; 
    
  }
}

#endif //DATASETIO_H
