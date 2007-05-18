#ifndef RECORDER_H
#define RECORDER_H

#include <dbus-c++/dbus.h>
#include <dbus-c++/glib-integration.h>
#include <dbus-c++/error.h>
#include "recorder-glue.h"
#include "h5filerecorder.h"
#include <network/network.h>

namespace soma { namespace recorder {
  
  
  class Recorder
    : public org::soma::Recorder, 
    public DBus::IntrospectableAdaptor,
    public DBus::ObjectAdaptor
    {
    public:
      Recorder(DBus::Connection & connection); 
      // DBUS-exposed methods
      void createFile(const DBus::String & filename); 
      void closeFile(); 
      void createEpoch(const DBus::String & epochname); 
      void switchEpoch(const DBus::String & epochname); 
      DBus::String getEpoch(); 
      void enableDataRX(int typ, int src); 
      void disableDataRX(int typ, int src); 
      void startRecording(); 
      void stopRecording(); 
    private: 
      std::auto_ptr<H5FileRecorder> pH5F; 
      std::auto_ptr<Network> pNetwork; 
      void newDataCallback(); 
      
    };
  
  
}
}

#endif // RECORDER_H
