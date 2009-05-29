


include_directories(${Boost_INCLUDE_DIRS})
include_directories(${EXPDEPS_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIRS})

INSTALL(PROGRAMS soma-recorder-manager DESTINATION bin)
