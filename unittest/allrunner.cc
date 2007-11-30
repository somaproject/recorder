#define BOOST_TEST_DYN_LINK
//#define BOOST_TEST_AUTO_MAIN
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <H5Cpp.h>
#include <iostream>



using namespace boost::unit_test; 

test_suite*
init_unit_test_suite( int , char* [] )
{
    std::cout << "DO YOU HAVE ANY IDEA WHAT YOURE DOING?" << std::endl; 

    H5::Exception::dontPrint();
    // with explicit registration we could specify a test case timeout
    //framework::master_test_suite().add( BOOST_TEST_CASE( &infinite_loop ), 0, /* timeout */ 2 );
    
    return 0; 
}
