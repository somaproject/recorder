
#include <unistd.h>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <sys/time.h>
#include <time.h>

#include "dbusrecorder.h"

using namespace soma::recorder; 
using namespace boost; 


DBusRecorder::DBusRecorder(DBus::Connection & connection, 
			   std::string somaIP, 
			   std::string experimentbin)
  : DBus::ObjectAdaptor(connection, "/soma/recording/recorder"), 
    somaIP_(somaIP), 
    experimentbin_(experimentbin), 
    logdbus_(log4cpp::Category::getInstance("soma.recording.recorder.dbus"))
{
  // How do we determine the working directory? 
  // 
  // recorder is really unique in that it only exists
  // to manage other DBUS objects and control their lifecycles. 
  // 
  // When you start one up, it connects back to recorder along the
  // "callback" interface to register itself as one of the open ones? 
  // 
  // sure, that sounds fine
  // 
  // thus we don't really have an interesting underlying interface
  // 

  
}




DBusRecorder::~DBusRecorder()
{

}


std::vector< ::std::string > DBusRecorder::ListOpenExperiments(  )
{

  std::vector<std::string> connections;
  connections.reserve(openExperiments_.size()); 

  std::map<std::string, std::string>::iterator i; 
  for (i = openExperiments_.begin(); i != openExperiments_.end(); i++) 
    {
      connections.push_back(i->second); 
    }
  return connections; 
}


void DBusRecorder::OpenExperiment( const ::std::string& name )
{
  logdbus_.infoStream() << "open experiment request " << name; 

  // set the relevant bits and call system for the 
  // target with command-line arguments
  // FIXME

}

void DBusRecorder::CreateExperiment( const ::std::string& name )
{

  // check if filename exists, and if so, raise error
  boost::filesystem::path  exppath(name); 
  logdbus_.infoStream() << "create experiment request " << name; 

  // get current category 
  log4cpp::Priority::Value priority = log4cpp::Category::getRoot().getPriority(); 
  int priorityval = priorityToInt(priority); 

  if (boost::filesystem::exists(exppath)) {
    // throw dbus error
    logdbus_.warnStream() << "file " << name << "already exists"; 

    throw DBus::Error("soma.Recorder", "File already exists"); 
  }


  logdbus_.debugStream() << "constructing command" ; 
  
  boost::format runstr("%s --create-file=%s --soma-ip=%s --log-level=%d"); 
  runstr % experimentbin_ % name % somaIP_ % priorityval; 

  logdbus_.debugStream() << "running experiment command '"
			 << boost::str(runstr) << "'";   
  

  int result = system(boost::str(runstr).c_str()); 
  logdbus_.debugStream() << "command done"; 

  if (result != 0) {
    boost::format errfmt("command %s returned %d error code"); 

    errfmt % boost::str(runstr) % result; 
    logdbus_.errorStream() << "experiment command returned error "
			   << result; 
      
    throw DBus::Error("soma.Recorder", boost::str(errfmt).c_str()); 
  }
  
  
}

std::vector< ::std::string > DBusRecorder::ListAvailableExperiments(  )
{
  
}

void DBusRecorder::RegisterExperiment( const ::std::string& filename, 
				       const ::std::string& dbusconn )
{

  

  logdbus_.infoStream() << "register from " << dbusconn
			<< " with file " << filename; 

  if (openExperiments_.find(filename) != openExperiments_.end()) {
    logdbus_.warnStream() << "file " << filename << " already open by " 
			  << dbusconn; 
  }
  
  openExperiments_[filename] = dbusconn; 
  experimentavailable(dbusconn); 
}

void DBusRecorder::UnregisterExperiment(const ::std::string& dbusconn )
{

  logdbus_.infoStream() << "unregister from " << dbusconn;
  bool found = false; 
  std::map<std::string, std::string>::iterator i, j; 
  for(i  = openExperiments_.begin();  i!= openExperiments_.end(); i++) {
    if(i->second == dbusconn) {
      found = true; 
      j = i; 
    }
  }
  if (!found) {
    logdbus_.warnStream() << "dbus path  " <<  dbusconn 
			  << "attempted unregister but cant be found"; 
  } else {
    openExperiments_.erase(j); 
  }

}

std::map< ::std::string, ::std::string > DBusRecorder::GetStatistics()
{
  // statistics to add to the file
  std::map< ::std::string, ::std::string > result; 

  result["somaip"] = somaIP_; 
  result["recorderip"] = "127.0.0.1"; 

  timeval tv; 
  gettimeofday(&tv, NULL);

  result["time"] = boost::str(boost::format("%f") % tv.tv_sec); 
  result["path"] = "127.0.0.1"; 
  result["cwd"] = "127.0.0.1"; 
  result["dbuspath"] = "127.0.0.1"; 
  result["diskfree"] = "127.0.0.1"; 
  result["version"] = "127.0.0.1"; 

  return result; 
}


