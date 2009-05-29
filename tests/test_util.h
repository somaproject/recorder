#ifndef TEST_UTIL_H
#define TEST_UTIL_H
#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp" 
#include <iostream>                        
#include <fstream>
#include "eventtable.h"
#include <boost/format.hpp>  
#include "test_config.h"


using namespace boost::filesystem; 

static const path test_binary_path(TEST_BINARY_PATH); 
static const path test_source_path(TEST_SOURCE_PATH); 

int run_standard_py_script(std::string scriptname, 
			   std::string command); 


#endif // TEST_UTIL_H
