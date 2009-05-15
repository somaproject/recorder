#include "dbusepoch.h"
#include <boost/format.hpp>
using namespace soma::recorder; 
using namespace boost; 

DBUSEpoch::DBUSEpoch(DBus::Connection & connection, 
		pEpoch_t epoch, int id) :
  DBus::ObjectAdaptor(connection, 
		      boost::str(boost::format("/soma/recording/experiment/%d") % id )),
  pEpoch_(epoch),
  dbusID_(id),
  logdbus_(log4cpp::Category::getInstance("soma.recording.epoch.dbus"))
{
  
}

DBUSEpoch::~DBUSEpoch()
{
  

}

::string DBUSEpoch::GetName()
{
  logdbus_.info("DBusEpoch::GetName()"); 
  return pEpoch_->getName(); 
  
}

void DBUSEpoch::StartRecording()
{
  logdbus_.info("DBusEpoch Start Recording"); 
  bool recstatusold= pEpoch_->isRecording(); 
  pEpoch_->startRecording(); 
  bool recstatusnew= pEpoch_->isRecording(); 

  if (recstatusold != recstatusnew) {
    logdbus_.info("DBusEpoch sending recording state change signal "); 
    
    recordingstate(recstatusnew); 
  }

}


void DBUSEpoch::StopRecording()
{
  logdbus_.info("DBusEpoch Stop Recording"); 
  bool recstatusold= pEpoch_->isRecording(); 
  pEpoch_->stopRecording(); 
  bool recstatusnew= pEpoch_->isRecording(); 

  if (recstatusold != recstatusnew) {
    logdbus_.info("DBusEpoch sending recording state change signal"); 

    recordingstate(recstatusnew); 
  }


}

void DBUSEpoch::EnableDataSink( const int32_t& src, const int32_t& typ )
{
  try {

    pEpoch_->enableDataSink(src, (datatype_t)typ); 
    sinkchange(src); 
  } catch (std::exception & e) {
    
    throw DBus::Error("soma.Epoch", "cannot modify data sink when recording or once recorded" ); 

  }

}

void DBUSEpoch::DisableDataSink( const int32_t& src, const int32_t& typ )
{
  try {
    pEpoch_->disableDataSink(src, (datatype_t)typ); 
    sinkchange(src); 
  } catch (std::exception & e) {
    throw DBus::Error("soma.Epoch", "cannot modify data sink when recording or once recorded" ); 

  }
}

std::vector< ::string > DBUSEpoch::GetDataSinks(  )
{
  std::vector< ::string> results; 
  
  std::set<dpair_t> sinks = pEpoch_->getDataSinks(); 
  
  for (std::set<dpair_t>::iterator s = sinks.begin(); 
       s != sinks.end(); s++) 
    {
      boost::format pair("(%d, %d)"); 
      pair % (int)(s->first) % (int)(s->second); 
      results.push_back(boost::str(pair));
      
    }
  
  return results; 

}
std::vector< int32_t > DBUSEpoch::GetDataSink( const int32_t& src)
{
  std::set<dpair_t> sinks = pEpoch_->getDataSinks(); 
  std::vector< int32_t> results; 
  for (std::set<dpair_t>::iterator s = sinks.begin(); 
       s != sinks.end(); s++) 
    {
      if (s->first == src) {
	results.push_back(s->second); 
	std::cout << "pushing back " <<(int)s->second << std::endl; 

      }
    }
  std::cout << "GetDataSink results size" << results.size() << " " << sinks.size() << std::endl; 
  return results; 
}


void DBUSEpoch::SetDataName( const int32_t& src, const ::string& name )
{
  try {
    pEpoch_->setDataName(src, name); 
    sinkchange(src); 

  } catch (std::exception & e) {
    throw DBus::Error("soma.Epoch", "cannot modify data sink name when recording or once recorded" ); 

  }


}


::string DBUSEpoch::GetDataName( const int32_t& src )
{

  std::string foo =   pEpoch_->getDataName(src); 

  return foo; 
}

std::vector< std::vector< int32_t > > DBUSEpoch::GetSessions(  )
{

  std::vector<std::vector<int> > results; 

  std::list<Session> sessions = pEpoch_->getSessions(); 

  for (std::list<Session>::iterator is = sessions.begin(); 
       is != sessions.end(); is++) {
    std::vector<int> sessioni; 
    sessioni.push_back(is->startts); 
    sessioni.push_back(is->starttime); 
    sessioni.push_back(is->endts); 
    sessioni.push_back(is->endtime); 
    results.push_back(sessioni); 
  }

  logdbus_.infoStream() << "DBusEpoch GetSessions returning "
			<< sessions.size() << " sessions"; 

  return results; 

}


std::map<std::string, std::string > 
DBUSEpoch::GetSinkSessionStatistics( const int32_t& src, const int32_t& typ, 
			      const int32_t& session)
{
  std::map<std::string, std::string > results; 
  
  std::vector<SinkStats >::iterator p; 

  std::vector<SinkStats> stats = pEpoch_->getSinkStatistics(src, charToDatatype(typ)); 

  
  results["pktCount"] =  str(format("%d") % stats[session].pktCount); 
  results["latestSeq"] = str(format("%d") % stats[session].latestSeq); 
  results["dupeCount"] = str(format("%d") % stats[session].dupeCount); 
  results["pendingCount"] = str(format("%d") % stats[session].pendingCount);  
  results["missingPacketCount"] = str(format("%d") % stats[session].missingPacketCount);  
  results["reTxRxCount"] = str(format("%d") % stats[session].reTxRxCount); 
  results["outOfOrderCount"] = str(format("%d") % stats[session].outOfOrderCount);  

  return results;   

}


std::map< ::string, ::string > 
DBUSEpoch::GetSessionStatistics( const int32_t& session )
{

  return pEpoch_->getSessionStatistics(session); 
  
}


void DBUSEpoch::AddEventRXMask( const int32_t& src, 
				const std::vector< int32_t >& cmdlist )
{
  
  std::vector<eventcmd_t> cmds; 
  cmds.reserve(cmdlist.size());
  for(std::vector<int32_t>::const_iterator i = cmdlist.begin(); 
      i != cmdlist.end(); ++i) {
    cmds.push_back(*i); 
    
  }
  
  
  pEpoch_->addEventRXMask(src, cmds); 

}

std::vector<int32_t> DBUSEpoch::GetEventRXMask( const int32_t& src)
{

  std::vector<eventcmd_t> resultlist = pEpoch_->getEventRXMask(src); 
  
  std::vector<int32_t> dbuslist; 
  dbuslist.reserve(resultlist.size()); 
  for(std::vector<eventcmd_t>::const_iterator i = resultlist.begin(); 
      i != resultlist.end(); i++) 
    {
      dbuslist.push_back(*i); 
    }
  return dbuslist; 

}

void DBUSEpoch::RemoveEventRXMask( const int32_t& src, 
				   const std::vector< int32_t >& cmdlist )
{
  std::vector<eventcmd_t> cmds; 
  cmds.reserve(cmdlist.size());
  for(std::vector<int32_t>::const_iterator i = cmdlist.begin(); 
      i != cmdlist.end(); ++i) {
    cmds.push_back(*i); 
    
  }
  
  

  pEpoch_->removeEventRXMask(src, cmds); 

}

bool DBUSEpoch::GetRecordingState()
{
  return pEpoch_->isRecording(); 
}

// notes work

int32_t DBUSEpoch::CreateNote()
{
  return pEpoch_->createNote(); 
}

void DBUSEpoch::GetNote( const int32_t& notehandle, ::string& name, uint64_t& createtime, 
			 uint64_t& createts, uint64_t& edittime, uint64_t& editts, std::vector< ::string >& tags )
{
  
}

void DBUSEpoch::SetNote( const int32_t& notehandle, const ::string& name, const ::string& notetext, const std::vector< ::string >& tags )
{

}

void DBUSEpoch::DeleteNote(const int32_t& nodehandle)
{

}

std::vector< int32_t > DBUSEpoch::GetNotes()
{


}
