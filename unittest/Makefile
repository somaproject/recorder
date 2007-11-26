CC=g++
SRCDIR = ../src/
CXXFLAGS=-I$(SRCDIR) -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D_POSIX_SOURCE -UNDEBUG -I../ -g `pkg-config --cflags dbus-c++-1 glibmm-2.4 gtkmm-2.4 gdkmm-2.4 glibmm-2.4 somanetwork-1.0`  -I/usr/local/include -I/usr/local/include/hdf5  -I/usr/local/include/boost-1_34_1/


LDFLAGS=-lboost_unit_test_framework-gcc41-mt -lhdf5_cpp -lhdf5 -lboost_thread-gcc41-mt  -lboost_filesystem-gcc41-mt  -L/usr/local/lib -lhdf5_hl `pkg-config --libs dbus-c++-1 gtkmm-2.4 gdkmm-2.4 glibmm-2.4 somanetwork-1.0`

all: runner

recorder.o: $(SRCDIR)/recorder.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/recorder.cc

recorder: recorder.o network.o datareceiver.o
$(SRCDIR)/recorder-glue.h: $(SRCDIR)/recorder-introspect.xml
	/usr/local/bin/dbusxx-xml2cpp $(SRCDIR)/recorder-introspect.xml --adaptor=$(SRCDIR)/recorder-glue.h

tspiketable.o: $(SRCDIR)/tspiketable.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/tspiketable.cc

wavetable.o: $(SRCDIR)/wavetable.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/wavetable.cc

h5filerecorder.o: $(SRCDIR)/h5filerecorder.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/h5filerecorder.cc

H5FileRecorder_create.o: h5filerecorder.o

TSpikeTable_test.o: TSpikeTable_test.cc

WaveTable_test.o: WaveTable_test.cc

runner: runner.o  tspiketable.o wavetable.o h5filerecorder.o H5FileRecorder_create.o TSpikeTable_test.o WaveTable_test.o



recorder_runner: runner.o tspiketable.o tspiketable_test.o

clean:
	rm *.o
