#include <boost/program_options.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>                        
#include <fstream>
#include <gtkmm.h>
#include <glibmm.h>
#include <unistd.h>

#include <dbus-c++/dbus.h>
#include <dbus-c++/glib-integration.h>


#include "dbusexperiment.h"
#include "dbusrecorderproxy.h"
#include "dbusrecordermanager.h"

#include "logging.h" 

using namespace soma; 
using namespace boost;       
using namespace boost::filesystem; 
using namespace std; 
namespace po = boost::program_options;

std::string LOGROOT("soma.recordering.experiment"); 


int main(int argc, char * argv[])
{
 
  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("create-file", po::value<string>(), "name of experiment file to create")
    ("open-file", po::value<string>(), "name of experiment file to open")
    ("soma-ip", po::value<string>(), "The IP of the soma hardware")
    ("request-dbus-name", po::value<string>(), "Request the target dbus name for debugging")
    ("no-register",  "register with primary recorder (soma.Recorder) on DBus")
    ("no-daemon", "do not fork as a dameon, but remain attached to the console")
    ;
  

  po::variables_map vm;

  desc.add(logging_desc()); 

  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    

  int loglevel = config_logging(vm, LOGROOT); 

  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }
  
  log4cpp::Category& logrecorder = log4cpp::Category::getInstance(LOGROOT);
  log4cpp::Category& logdbus = log4cpp::Category::getInstance(LOGROOT + ".dbus");
  

  if (vm.count("create-file") and vm.count("open-file")) {
    logrecorder.fatal("Cannot both create and open a file"); 
    return -1; 
  } else if (not (vm.count("create-file") or vm.count("open-file"))) {
    logrecorder.fatal("Must specify either create-file or open-file"); 
    return -1; 
  }

  std::string filename; 
  bool create  = false; 
  if (vm.count("create-file")) {
    filename = vm["create-file"].as<string>(); 
    create = true; 
  } 

  if (vm.count("open-file")) {
    filename = vm["open-file"].as<string>(); 
    create = false; 
  } 
  if (create) {
    logrecorder.infoStream() << "create file: " << filename; 
  } else {
    logrecorder.infoStream() << "open file: " << filename; 
  }    

  std::string somaip; 
  if (!vm.count("soma-ip")) {
    logrecorder.fatal("soma-ip was not specified, no way to get data"); 
    return -1; 
  }
  somaip = vm["soma-ip"].as<string>(); 

  logrecorder.infoStream() << "soma hardware IP: " << somaip; 

  // by default, we try and discover and use the link-local soma bus

  DBus::Glib::BusDispatcher dispatcher;
  dispatcher.set_priority(1000); 
  Glib::RefPtr<Glib::MainLoop> mainloop;

  DBus::default_dispatcher = &dispatcher;
  dispatcher.attach(NULL); 
  
  // FIXME get the right bus! 
  DBus::Connection conn = DBus::Connection::SessionBus();

  if (vm.count("request-dbus-name")) {
    std::string dbusname = vm["request-dbus-name"].as<string>(); 
    conn.request_name(dbusname.c_str()); 
    logdbus.infoStream() << "requesting dbus name" << dbusname; 
  }

  

  if (! vm.count("no-daemon")) {
    pid_t pid = fork(); 
    if (pid == 0) {
      logrecorder.infoStream() << "daemon process running"; 
    } else {
      // parent
      return 0; 
    }
  } else {

    logdbus.warnStream() << "not forking daemon as requested"; 
  }

  mainloop = Glib::MainLoop::create(); 

  soma::recorder::DBUSExperiment server(conn, mainloop, 
					filename, somaip, create); 
  logdbus.infoStream() << "running with path=" << server.path(); 


  // Must do DBUS stuff AFTER fork so that recorder can respond

  soma::recorder::DBusRecorderManager * precorder; 
  // now optionally register
  if (vm.count("no-register")) {
    // do not register
    logdbus.warnStream() << "not registering experiment with DBus"; 
  } else {
    precorder = new soma::recorder::DBusRecorderManager(conn, "/soma/recording/recorder", 
							"soma.recording.Recorder"); 
    precorder->RegisterExperiment(filename, conn.unique_name()); 
    logdbus.infoStream() << "dbus registered with " 
			 << filename  << conn.unique_name(); 
    
  }
  

  mainloop->run(); 
  
}
