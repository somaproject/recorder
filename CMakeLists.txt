CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(tspikes)

find_package(Boost 1.37.0 COMPONENTS 
  thread unit_test_framework filesystem program_options system REQUIRED)
find_package(PkgConfig)
set(CMAKE_MODULE_PATH CMake)

find_package(HDF5 REQUIRED)

pkg_check_modules(EXPDEPS REQUIRED 
  sigc++-2.0>=2.0 
  dbus-c++-1>=0.5
  glibmm-2.4>=2.13.0
  gtkmm-2.4>=2.10
  somanetwork-1.0>=1.0
  log4cpp>=1.0
)

include_directories(src)

ENABLE_TESTING()
add_subdirectory(src)
add_subdirectory(unittest)
add_subdirectory(benchmarks)
