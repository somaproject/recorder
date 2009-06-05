include_directories(${Boost_INCLUDE_DIRS})
include_directories(${EXPDEPS_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIRS})

add_library(test_util
  test_util.cc
)

add_executable(H5FileCreate_test
  runner.cc
  H5FileCreate.cc
)

add_executable(H5Experiment_test
  H5Experiment_recording.cc
  H5Experiment.cc
  runner.cc
)

add_executable(NoteTable_test
  runner.cc
  NoteTable_test.cc
)

add_executable(TSpikeTable_test
  runner.cc
  TSpikeTable_test.cc
)

add_executable(EventTable_test
  runner.cc
  EventTable_test.cc
)

add_executable(WaveTable_test
  runner.cc
  WaveTable_test.cc
)

add_executable(EventMaskSet_test
  runner.cc
  EventMaskSet_test.cc
)

add_executable(H5Epoch_test
  runner.cc
  H5Epoch_appenddata.cc
  H5Epoch_test.cc
  )

set(testlist 
  H5FileCreate_test
  NoteTable_test
  TSpikeTable_test
  EventTable_test
  WaveTable_test
  EventMaskSet_test
  H5Epoch_test
  H5Experiment_test
)

foreach(testname ${testlist})
  
  target_link_libraries(${testname}
    experiment_core
    test_util
    ${Boost_LIBRARIES} 
    ${EXPDEPS_LIBRARIES}
    )
  
  ADD_TEST(${testname} ${testname})

endforeach(testname)


SET(TEST_SOURCE_PATH "${PROJECT_SOURCE_DIR}/tests")
SET(TEST_BINARY_PATH "${PROJECT_BINARY_DIR}/tests")

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/tests/test_config.h.in
  ${PROJECT_BINARY_DIR}/tests/test_config.h)
include_directories("${PROJECT_BINARY_DIR}/tests")



add_subdirectory(dbus)
