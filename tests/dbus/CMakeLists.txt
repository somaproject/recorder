CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/runtests.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/runtests.py)

ADD_TEST("dbus-server-python-tests" "python" "runtests.py" )
