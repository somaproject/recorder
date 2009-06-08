
include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/ext-logging/src" 
  ${Boost_INCLUDE_DIRS}
  ${EXPDEPS_INCLUDE_DIRS}
  ${HDF5_INCLUDE_DIRS}
)

link_directories(
  ${Boost_LIBRARY_DIRS}
  ${EXPDEPS_LIBRARY_DIRS}
  ${HDF5_LIBRARY_DIRS}
)

message(STATUS   ${EXPDEPS_LIBRARY_DIRS})

set(logging_SRCS 
  ext-logging/src/logging.cc
)


set(experiment_SRCS
  h5experiment.cc 
  h5epoch.cc 
  tspiketable.cc
  wavetable.cc 
  eventtable.cc 
  eventmaskset.cc 
  notetable.cc)

set(dbus_experiment_SRCS
  dbusexperiment.cc dbusepoch.cc
  dbusrecordermanager.cc
)

set(dbus_recorder_SRCS
  dbusrecorder.cc
)

ADD_LIBRARY(experiment_core
  ${experiment_SRCS} 
  ${logging_SRCS}
  )

TARGET_LINK_LIBRARIES(experiment_core
  ${Boost_LIBRARIES} 
  ${HDF5_LIBRARIES}
  ${HDF5_CPP_LIBRARIES}
  ${HDF5_HL_LIBRARIES}
  ${EXPDEPS_LIBRARIES}
)

ADD_EXECUTABLE(soma-recorder-experiment
  experimentmain.cc
  ${dbus_experiment_SRCS} 
  ${logging_SRCS}
  )

TARGET_LINK_LIBRARIES(soma-recorder-experiment
  experiment_core
  ${Boost_LIBRARIES} 
  )

INSTALL(PROGRAMS soma-recorder-experiment DESTINATION bin)
