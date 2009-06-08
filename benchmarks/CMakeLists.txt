include_directories(${Boost_INCLUDE_DIRS})
include_directories(${EXPDEPS_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIRS})
link_directories(
  ${Boost_LIBRARY_DIRS}
  ${EXPDEPS_LIBRARY_DIRS}
  ${HDF5_LIBRARY_DIRS}
)

add_executable(h5experiment_writer
  h5experiment_writer.cc
)

target_link_libraries(h5experiment_writer
  experiment_core
  ${Boost_LIBRARIES} 
  ${EXPDEPS_LIBRARIES}
  )
