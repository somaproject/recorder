
#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>                        
#include <fstream>
#include <gtkmm.h>
#include <glibmm.h>

#include "recorder.h"

using namespace soma; 
using namespace boost;       
using namespace boost::filesystem; 
using namespace std; 



int main()
{
  
  DBus::Glib::BusDispatcher dispatcher;
  Glib::RefPtr<Glib::MainLoop> mainloop;

  DBus::default_dispatcher = &dispatcher;
  dispatcher.attach(NULL); 
  
  DBus::Connection conn = DBus::Connection::SessionBus();
  conn.request_name("org.soma.Recorder"); 
  soma::recorder::Recorder server(conn);
  mainloop = Glib::MainLoop::create(); 
  mainloop->run(); 
  

}
