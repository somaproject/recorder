include_directories(${Boost_INCLUDE_DIRS})
include_directories(${EXPDEPS_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIRS})

add_executable(H5FileCreate_test
  runner.cc
  H5FileCreate.cc
)

add_executable(NoteTable_test
  runner.cc
  NoteTable.cc
)

add_executable(TSpikeTable_test
  runner.cc
  TSpikeTable_test.cc
)

set(testlist 
  H5FileCreate_test
  NoteTable_test
  TSpikeTable_test )

foreach(testname ${testlist})
  
  target_link_libraries(${testname}
    experiment_core
    ${Boost_LIBRARIES} 
    ${EXPDEPS_LIBRARIES}
    )
  
  ADD_TEST(${testname} ${testname})

endforeach(testname)

