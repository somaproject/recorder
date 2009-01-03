#include <boost/program_options.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>                        
#include <fstream>
#include <gtkmm.h>
#include <glibmm.h>
#include <string>

#include "dbusrecorder.h"
#include <dbus-c++/dbus.h>
#include <dbus-c++/glib-integration.h>

#include "logging.h"

using namespace soma; 
using namespace boost;       
using namespace boost::filesystem; 
using namespace std; 
namespace po = boost::program_options;

std::string LOGROOT("soma.recording.recorder"); 

int main(int argc, char * argv[])
{
 
  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("soma-ip", po::value<string>(), "The IP of the soma hardware")
    ("experimentbin", 
     po::value<string>()->default_value("soma-experiment"),
     "The program to use as the experiment server. Defaults to 'soma-experiment")
    ("request-dbus-name", po::value<string>()->default_value("soma.recording.Recorder"), 
     "Request the target dbus name for debugging"); 
    

  desc.add(logging_desc()); 
  
  
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    
  
  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }
  
  config_logging(vm, LOGROOT); 

  // get logging objects
  log4cpp::Category& logrecorder = log4cpp::Category::getInstance(LOGROOT);
  log4cpp::Category& logdbus = log4cpp::Category::getInstance(LOGROOT + ".dbus");
  
  std::string somaip; 
  if (!vm.count("soma-ip")) {
    logrecorder.fatal("soma-ip was not specified, no way to get data"); 
  }

  somaip = vm["soma-ip"].as<string>(); 
  logrecorder.infoStream() << "soma hardware IP: " << somaip; 


  // by default, we try and discover and use the link-local soma bus

  DBus::Glib::BusDispatcher dispatcher;
  dispatcher.set_priority(1000); 

  Glib::RefPtr<Glib::MainLoop> mainloop;

  DBus::default_dispatcher = &dispatcher;
  dispatcher.attach(NULL); 
  
  DBus::Connection conn = DBus::Connection::SessionBus();

  logrecorder.infoStream() << "experiment subprocess binary: " << vm["experimentbin"].as<string>();   

  soma::recorder::DBusRecorder server(conn, somaip, 
				      vm["experimentbin"].as<string>()); 
  
  std::string dbusname = vm["request-dbus-name"].as<string>(); 
  logdbus.infoStream() << "dbus name: " << dbusname; 

  conn.request_name(dbusname.c_str()); 

  mainloop = Glib::MainLoop::create(); 
  mainloop->run(); 
  
}
