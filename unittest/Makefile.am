# 


AM_LDFLAGS = $(GTKDEPS_LIBS) $(BOOST_LDFLAGS) $(SOMADEPS_LIBS) \
	$(BOOST_THREAD_LIB)  $(BOOST_PROGRAM_OPTIONS_LIB) \
	$(BOOST_UNIT_TEST_FRAMEWORK_LIB)


AM_CXXFLAGS = $(GTKDEPS_CFLAGS) -I@top_srcdir@/src/  $(BOOST_CPPFLAGS) $(SOMADEPS_CFLAGS)  -pthread 


test_sources = H5Experiment_recording.cc H5Epoch.cc H5Experiment.cc \
		EventMaskSet.cc EventTable.cc TSpikeTable.cc WaveTable.cc \
		H5FileCreate.cc NoteTable.cc

check_PROGRAMS = runner runner-expfile runner-notetable
runner_SOURCES =  runner.cc  $(test_sources)
runner_LDADD = @top_srcdir@/src/libtest.la

runner_expfile_SOURCES =  runner.cc  H5FileCreate.cc
runner_expfile_LDADD = @top_srcdir@/src/libtest.la


runner_notetable_SOURCES =  runner.cc  NoteTable.cc
runner_notetable_LDADD = @top_srcdir@/src/libtest.la


#http://www.gnu.org/software/autoconf/manual/autoconf-2.57/html_chapter/autoconf_17.html#SEC179  FAQ entry for how to handle the data directory


