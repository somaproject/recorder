#ifndef DBUSEXPERIMENT_H
#define DBUSEXPERIMENT_H

#include <somanetwork/networkinterface.h>

#include <dbus-c++/dbus.h>
#include <glibmm.h>

#include "experiment.h"
#include "logging.h"

#include "dbusexperimentadaptor.h"
#include "dbusepoch.h"

namespace soma {
  namespace recorder {
    class DBUSExperiment : 
      public  soma::recording::Experiment, 
      public  soma::recording::Notes, 
      public DBus::IntrospectableAdaptor,
      public DBus::ObjectAdaptor

    {
    public:
      DBUSExperiment(DBus::Connection & connection, 
		       Glib::RefPtr<Glib::MainLoop> ml, 
		     std::string filename, std::string somaIP, 
		     bool create); 
      
      std::map< std::string, std::string > GetFileProperties(  );
      std::vector< DBus::Path > GetEpochs(  );
      DBus::Path CreateEpoch( const std::string& name );
      std::string GetName(); 

      DBus::Path GetEpoch( const std::string& name ); 
      void RenameEpoch( const ::DBus::Path& epoch, const std::string& newname );
      void Close(); 

      // Note interface
      int32_t CreateNote(); 
      void GetNote( const int32_t& notehandle, std::string& name, uint64_t& createtime, uint64_t& createts, uint64_t& edittime, uint64_t& editts, std::vector< std::string >& tags ); 
      
      void SetNote( const int32_t& notehandle, const std::string& name, const std::string& notetext, const std::vector< std::string >& tags ); 
      void DeleteNote(const int32_t& nodehandle); 
      std::vector< int32_t > GetNotes(); 


    private:
      pExperiment_t pExperiment_; 
      pNetworkInterface_t pNetwork_; 
      
      std::vector<DBUSEpoch *> dbusEpochs_; 
      log4cpp::Category& logdbus_; 
      Glib::RefPtr<Glib::MainLoop> glibmainloop_; 
      
      DBus::Connection & connection_; 
      int nextEpochDBusID_; 
    }; 
    
  }

}

#endif // DBUSEXPERIMENT_H
