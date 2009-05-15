# 

AM_CPPFLAGS = $(BOOST_CPPFLAGS) $(BOOST_THREAD_CPPFLAGS) \
	$(BOOST_FORMAT_CPPFLAGS) $(SOMADEPS_CFLAGS)  $(GTKDEPS_CFLAGS) $(HDF5_CFLAGS) -pthread
AM_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS) \
	$(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS) $(GTKDEPS_LDFLAGS) \
	$(HDF5_LDFLAGS)  $(SOMADEPS_LDFLAGS)

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
soma_recorder_LDADD = libtest.la  $(BOOST_THREAD_LIBS) $(BOOST_SYSTEM_LIBS) \
	$(BOOST_PROGRAM_OPTIONS_LIBS) $(GTKDEPS_LIBS)  $(HDF5_LIBS) $(SOMADEPS_LIBS) \
	$(BOOST_FILESYSTEM_LIBS)

soma_recorder_CPPFLAGS = $(AM_CPPFLAGS)

soma_experiment_SOURCES = experimentmain.cc $(dbus_sources)
soma_experiment_LDADD = libtest.la  $(BOOST_THREAD_LIBS) $(BOOST_SYSTEM_LIBS) \
	$(BOOST_PROGRAM_OPTIONS_LIBS)  $(HDF5_LIBS) $(SOMADEPS_LIBS) \
	$(BOOST_FILESYSTEM_LIBS)  $(GTKDEPS_LIBS) 

libtest_la_SOURCES = $(cc_sources) 
libtest_la_CPPFLAGS = $(AM_CPPFLAGS)
libtest_la_LIBS = $(BOOST_THREAD_LIBS) $(BOOST_SYSTEM_LIBS) \
	 $(BOOST_PROGRAM_OPTIONS_LIBS)  $(HDF5_LIBS) $(SOMADEPS_LIBS)
#http://www.gnu.org/software/autoconf/manual/autoconf-2.57/html_chapter/autoconf_17.html#SEC179  FAQ entry for how to handle the data directory


