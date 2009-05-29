#include "test_util.h"

int run_standard_py_script(std::string scriptname, 
			   std::string command) 
{

  path scriptpath = test_source_path / scriptname; 
  boost::format sysfmt("python %s %s %s" );
  std::string sysstr = boost::str(sysfmt % scriptpath.string()
				  % command 
				  % test_binary_path.string()
				  ); 
  int retval = std::system(sysstr.c_str()); 
  return retval; 

}

