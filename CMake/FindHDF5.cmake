#
# Find the native HDF5 includes and library
# Stolen from the paraview repo on github
# HDF5_INCLUDE_DIR - where to find H5public.h, etc.
# HDF5_LIBRARIES - List of fully qualified libraries to link against when using hdf5.
# HDF5_FOUND - Do not attempt to use hdf5 if "no" or undefined.
 
FIND_PATH(HDF5_INCLUDE_DIR H5public.h
  /usr/local/include
  /usr/include
)
 
FIND_LIBRARY(HDF5_LIBRARY hdf5 
  /usr/local/lib
  /usr/lib
)
 
FIND_LIBRARY(HDF5_HL_LIBRARY hdf5_hl
  /usr/local/lib
  /usr/lib
)
 
FIND_LIBRARY(HDF5_CPP_LIBRARY hdf5_cpp
  /usr/local/lib
  /usr/lib
)
 
IF(HDF5_INCLUDE_DIR)
  IF(HDF5_LIBRARY)
    SET( HDF5_LIBRARIES ${HDF5_LIBRARY} )
    SET( HDF5_FOUND "YES" )
  ENDIF(HDF5_LIBRARY)
ENDIF(HDF5_INCLUDE_DIR)

IF(HDF5_CPP_LIBRARY)
  SET(HDF5_CPP_LIBRARIES ${HDF5_CPP_LIBRARY})
ENDIF(HDF5_CPP_LIBRARY)

IF(HDF5_HL_LIBRARY)
  SET(HDF5_HL_LIBRARIES ${HDF5_HL_LIBRARY})
ENDIF(HDF5_HL_LIBRARY)

IF (HDF5_FOUND) 
ELSE(NOT HDF5_FOUND) 
  IF (HDF5_FIND_REQUIRED)
    message(SEND_ERROR "Unable to find the required HDF5 Libraries")
  ENDIF(HDF5_FIND_REQUIRED)
ENDIF(HDF5_FOUND)  
