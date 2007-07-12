#ifndef RECORDER_H
#define RECORDER_H

#include <dbus-c++/dbus.h>
#include <dbus-c++/glib-integration.h>
#include <dbus-c++/error.h>
#include <gtkmm/main.h>

#include "recorder-glue.h"
#include "h5filerecorder.h"
#include <somanetwork/network.h>

namespace soma { namespace recorder {
  
  class Recorder
    : public org::soma::Recorder, 
    public DBus::IntrospectableAdaptor,
    public DBus::ObjectAdaptor
    {
    public:
      Recorder(DBus::Connection & connection); 
      // DBUS-exposed methods
      void CreateFile(const DBus::String & filename); 
      void CloseFile(); 
      void CreateEpoch(const DBus::String & epochname); 
      void SwitchEpoch(const DBus::String & epochname); 
      DBus::String GetEpoch(); 
      void EnableDataRX(const DBus::Int32 & typ, const DBus::Int32 & src); 
      void DisableDataRX(const DBus::Int32 & typ, const DBus::Int32 & src); 
      void StartRecording(); 
      void StopRecording(); 
      std::map< DBus::String, DBus::String > NetworkStats();
      
    private: 
      std::auto_ptr<H5FileRecorder> pH5F_; 
      std::auto_ptr<Network> pNetwork_; 
      bool newDataCallback(Glib::IOCondition io_condition); 
      bool newEventCallback(Glib::IOCondition io_condition); 
      sigc::connection sigNewData_; 
      sigc::connection sigNewEvent_; 
      bool recording_ ; 
      void checkRecording(bool isRecording); 

    };
  
  
}}

#endif // RECORDER_H
