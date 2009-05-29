
# first just for soma
INSTALL(FILES __init__.py 
	       DESTINATION ${PY_SOMA_TARGET})

# then for recorder
INSTALL(FILES recorder/__init__.py recorder/manager.py
  DESTINATION "${PY_SOMA_TARGET}/recorder")

