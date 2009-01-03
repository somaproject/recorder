# 


AM_LDFLAGS = $(GTKDEPS_LIBS) $(BOOST_LDFLAGS) $(HDF5_LIBS)  $(SOMADEPS_LIBS) \
	$(BOOST_THREAD_LIB)  $(BOOST_PROGRAM_OPTIONS_LIB) $(BOOST_FILESYSTEM_LIB)


AM_CXXFLAGS = $(GTKDEPS_CFLAGS)  $(BOOST_CPPFLAGS) \
	$(HDF5_CFLAGS)  $(SOMADEPS_CFLAGS)  -pthread 


h_sources = experiment.h h5experiment.h	logging.h tspiketable.h wavetable.h \
	sinkstats.h h5helper.h datasetio.h epoch.h h5epoch.h  eventmaskset.h \
	notetable.h

cc_sources = h5experiment.cc h5epoch.cc tspiketable.cc \
	wavetable.cc eventtable.cc logging.cc eventmaskset.cc notetable.cc

dbus_headers = dbusexperiment.h dbusexperimentadaptor.h \
		dbusepoch.h dbusepochadaptor.h \
		 dbusrecorderadaptor.h dbusrecorder.h \
		dbusrecordermanager.h dbusrecorderadaptor.h 

dbus_sources = dbusexperiment.cc dbusepoch.cc dbusrecorder.cc \
	dbusrecordermanager.cc 		

noinst_LTLIBRARIES = libtest.la



bin_PROGRAMS = soma-recorder soma-experiment
soma_recorder_SOURCES =  recordermain.cc $(dbus_sources)
soma_recorder_LDADD = libtest.la

soma_experiment_SOURCES = experimentmain.cc $(dbus_sources)
soma_experiment_LDADD = libtest.la

libtest_la_SOURCES = $(cc_sources) 
libtest_la_CXXFLAGS = $(AM_CXXFLAGS)
#http://www.gnu.org/software/autoconf/manual/autoconf-2.57/html_chapter/autoconf_17.html#SEC179  FAQ entry for how to handle the data directory


