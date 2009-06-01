#ifndef DBUSEPOCH_H
#define DBUSEPOCH_H

#include <somanetwork/networkinterface.h>

#include <dbus-c++/dbus.h>

#include "experiment.h"
#include "epoch.h"
#include "logging.h"

#include "dbusepochadaptor.h"
#include "dbusexperimentadaptor.h"


namespace soma {
  namespace recorder {
    class DBUSEpoch : 
      public  soma::recording::Epoch_adaptor, 
      public  soma::recording::Notes_adaptor, 

      public DBus::IntrospectableAdaptor,
      public DBus::ObjectAdaptor

    {
    public:
      DBUSEpoch(DBus::Connection & connection, 
		pEpoch_t, int id); 
      
      std::string GetName(  ); 

      void StartRecording(); 
      void StopRecording(); 
      bool GetRecordingState(); 
      
      void EnableDataSink( const int32_t& src, const int32_t& typ );
      void DisableDataSink( const int32_t& src, const int32_t& typ );
      std::vector< std::string > GetDataSinks(); 
      std::vector< int32_t > GetDataSink( const int32_t& src); 
      
      void SetDataName( const int32_t& src, const std::string& name ); 
      std::string GetDataName( const int32_t& src ); 
      std::vector< std::vector< int32_t > > GetSessions(  ); 
      
      std::map<std::string, std::string > 
      GetSinkSessionStatistics( const int32_t& src, const int32_t& typ, 
				 const int32_t& session); 
      
      std::map< std::string, std::string > GetSessionStatistics( const int32_t& session ); 


      //std::map< std::string, std::string > GetFileProperties(  );
      //std::vector< DBus::Path > GetEpochs(  );
      //DBus::Path CreateEpochs( const std::string& name );

      
      void AddEventRXMask( const int32_t& src, const std::vector< int32_t >& cmdlist );
      std::vector< int32_t > GetEventRXMask( const int32_t& src ); 
      void RemoveEventRXMask( const int32_t& src, const std::vector< int32_t >& cmdlist ); 


      // Note interface
      int32_t CreateNote(); 
      void GetNote( const int32_t& notehandle, std::string& name, uint64_t& createtime, uint64_t& createts, uint64_t& edittime, uint64_t& editts, std::vector< std::string >& tags ); 
      
      void SetNote( const int32_t& notehandle, const std::string& name, const std::string& notetext, const std::vector< std::string >& tags ); 
      void DeleteNote(const int32_t& nodehandle); 
      std::vector< int32_t > GetNotes(); 



      ~DBUSEpoch();
      
      pEpoch_t getEpochPointer() {
	return pEpoch_;
      }

    private:
      pEpoch_t pEpoch_; 
      int dbusID_; 
      log4cpp::Category& logdbus_; 

    }; 
    
  }

}

#endif // DBUSEPOCH_H
