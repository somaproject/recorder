
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${EXPDEPS_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIRS})

set(experiment_SRCS
  h5experiment.cc h5epoch.cc tspiketable.cc
  wavetable.cc eventtable.cc logging.cc eventmaskset.cc notetable.cc)

set(dbus_experiment_SRCS
  dbusexperiment.cc dbusepoch.cc
  dbusrecordermanager.cc

)

set(dbus_recorder_SRCS
  dbusrecorder.cc
)

ADD_LIBRARY(experiment_core
  ${experiment_SRCS} 
  )

TARGET_LINK_LIBRARIES(experiment_core
  ${HDF5_LIBRARIES}
  ${HDF5_CPP_LIBRARIES}
  ${HDF5_HL_LIBRARIES}
)

ADD_EXECUTABLE(soma-experiment
  experimentmain.cc
  ${dbus_experiment_SRCS} 
  )

TARGET_LINK_LIBRARIES(soma-experiment
  experiment_core
  ${Boost_LIBRARIES} 
  ${EXPDEPS_LIBRARIES}
  )
