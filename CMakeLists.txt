CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(recorder)

IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)

find_package(Boost 1.37.0 COMPONENTS 
  thread unit_test_framework filesystem program_options system REQUIRED)
find_package(PkgConfig)
find_package(PythonInterp)
find_package(PythonLibs)
find_package(PythonLibrary)
include(PythonMacros)

find_package(HDF5 1.8.3 REQUIRED HDF5_HL HDF5_CPP)

pkg_check_modules(EXPDEPS REQUIRED 
  sigc++-2.0>=2.0 
  dbus-c++-1>=0.5
  glibmm-2.4>=2.13.0
  gtkmm-2.4>=2.10
  somanetwork-1.0>=1.0
  log4cpp>=1.0
)

include_directories(experiment
  "experiment/ext-logging/src" 
)
if(CMAKE_INSTALL_PREFIX) 
  set(PY_SOMA_TARGET "${CMAKE_INSTALL_PREFIX}/lib/python/")
else(CMAKE_INSTALL_PREFIX)
  set(PY_SOMA_TARGET "${PYTHON_SITE_PACKAGES_DIR}/soma_apps/")
endif(CMAKE_INSTALL_PREFIX)

MESSAGE(STATUS "The soma python recording module will be installed in " ${PY_SOMA_TARGET})

ENABLE_TESTING()
add_subdirectory(experiment)
add_subdirectory(manager)
add_subdirectory(soma_apps)
add_subdirectory(tests)
add_subdirectory(benchmarks)

link_directories(
  ${Boost_LIBRARY_DIRS}
  ${EXPDEPS_LIBRARY_DIRS}
  ${HDF5_LIBRARY_DIRS}
)
