#ifndef DBUSRECORDER_H
#define DBUSRECORDER_H


#include <dbus-c++/dbus.h>

#include "logging.h"


#include "dbusrecorderadaptor.h"

namespace soma {
  namespace recorder {
    class DBusRecorder: 
      public soma::recording::Recorder, // actually the dbus adaptor, 
      public soma::recording::ExperimentManager, 
      public DBus::IntrospectableAdaptor,
      public DBus::ObjectAdaptor

    {
    public:
      DBusRecorder(DBus::Connection & connection, 
		   std::string somaIP, std::string expbin); 
      ~DBusRecorder(); 

    private:
      // dbus methods
      std::vector< std::string > ListOpenExperiments(  ); 
      void OpenExperiment( const ::std::string& name ); 
      void CreateExperiment( const ::std::string& name ); 
      std::vector< ::std::string > ListAvailableExperiments(  ); 

      // registration/unregistration
      void RegisterExperiment( const ::std::string& filename, const std::string& dbuspath ); 
      void UnregisterExperiment( const std::string& dbuspath ); 


      std::map< ::std::string, ::std::string > GetStatistics(); 

      log4cpp::Category& logdbus_; 
      
      // pass
      std::string somaIP_; 
      std::string experimentbin_; 

      std::map<std::string, std::string> openExperiments_; 

    }; 
    
  }
}


#endif // DBUSRECORDER_H
