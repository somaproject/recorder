#include <iostream>

#include "dbusrecordermanager.h"

using namespace soma::recorder; 

DBusRecorderManager::DBusRecorderManager(DBus::Connection & conn, 
					 const char * path, const char * name) :
  DBus::ObjectProxy(conn, path, name)
{

  std::cout << "dbusRecorderManager created" << std::endl; 

}
