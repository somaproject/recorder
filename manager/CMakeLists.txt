


include_directories(${Boost_INCLUDE_DIRS})
include_directories(${EXPDEPS_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIRS})

INSTALL(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/soma-recorder-manager DESTINATION bin)
