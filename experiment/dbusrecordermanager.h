#ifndef DBUSRECORDERMANAGER
#define DBUSRECORDERMANAGER
#include "dbusrecorderproxy.h" 

namespace soma {
  namespace recorder {
    class DBusRecorderManager : 
      public soma::recording::ExperimentRegistry_proxy, 
      public DBus::IntrospectableProxy, 
      public DBus::ObjectProxy {
      
      
    public :
      DBusRecorderManager(DBus::Connection & conn, const char * path, const char * name); 
      

    }; 
  }
}

#endif // DBUSRECORDERMANAGER
