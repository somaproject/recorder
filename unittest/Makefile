CC=g++
SRCDIR = ../src/
CXXFLAGS=-I/home/jonas/hdf5/include -I$(SRCDIR) -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D_POSIX_SOURCE -UNDEBUG -I../ -g `pkg-config --cflags dbus-c++-1 glibmm-2.4 gtkmm-2.4 gdkmm-2.4 glibmm-2.4 somanetwork-1.0`  -I/usr/local/include -I/usr/local/include/boost-1_34_1/


LDFLAGS=-L/home/jonas/hdf5/lib -lboost_unit_test_framework -lhdf5_cpp -lhdf5 -lboost_thread  -lboost_filesystem  -L/usr/local/lib -lhdf5_hl `pkg-config --libs dbus-c++-1 gtkmm-2.4 gdkmm-2.4 glibmm-2.4 somanetwork-1.0`

all: runner

simpletest: simpletest.o


H5FileInterface_create.o: h5fileinterface.o

H5Epoch_test.o: h5epoch.o

H5Epoch_appenddata.o: h5epoch.o

h5fileinterface.o: $(SRCDIR)/h5fileinterface.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/h5fileinterface.cc

h5epoch.o: $(SRCDIR)/h5epoch.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/h5epoch.cc

tspiketable.o: $(SRCDIR)/tspiketable.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/tspiketable.cc

notetable.o: $(SRCDIR)/notetable.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/notetable.cc

wavetable.o: $(SRCDIR)/wavetable.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/wavetable.cc

eventtable.o: $(SRCDIR)/eventtable.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/eventtable.cc


# eventtable.o EventTable_test.o 

allrunner: allrunner.o \
	h5fileinterface.o H5FileInterface_create.o  \
	h5epoch.o H5Epoch_appenddata.o H5Epoch_test.o \
	eventtable.o EventTable_test.o \
	tspiketable.o TSpikeTable_test.o \
	wavetable.o WaveTable_test.o \
	notetable.o NoteTable_test.o


runner: runner.o  wavetable.o WaveTable_test.o





clean:
	rm *.o
