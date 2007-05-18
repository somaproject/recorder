
SRCDIR = ../src/
CXXFLAGS=-I$(SRCDIR) -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D_POSIX_SOURCE -UNDEBUG -I../ -g `pkg-config --cflags dbus-c++-1 glibmm-2.4 gtkmm-2.4 gdkmm-2.4 glibmm-2.4`

LDFLAGS=-lboost_unit_test_framework -lboost_thread  -lhdf5_cpp -lhdf5  -lboost_filesystem -lhdf5_hl `pkg-config --libs dbus-c++-1 gtkmm-2.4 gdkmm-2.4 glibmm-2.4`

all: recorder_runner

recorder.o: $(SRCDIR)/recorder.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/recorder.cc

recorder: recorder.o network.o datareceiver.o
$(SRCDIR)/recorder-glue.h: $(SRCDIR)/recorder-introspect.xml
	/usr/local/bin/dbusxx-xml2cpp $(SRCDIR)/recorder-introspect.xml --adaptor=$(SRCDIR)/recorder-glue.h

network.o: ../network/network.cc
	$(CXX) $(CXXFLAGS) -c ../network/network.cc

datareceiver.o: ../network/network.cc
	$(CXX) $(CXXFLAGS) -c ../network/datareceiver.cc

tspiketable.o: $(SRCDIR)/tspiketable.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/tspiketable.cc


h5filerecorder.o: $(SRCDIR)/h5filerecorder.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/h5filerecorder.cc

H5FileRecorder_create.o: h5filerecorder.o

tspiketable_test.o: tspiketable_test.cc

runner: runner.o  tspiketable.o tspiketable_test.o h5filerecorder.o

recorder_runner: runner.o recorder.o recorder_test.o network.o datareceiver.o h5filerecorder.o tspiketable.o

	$(CXX) $(LDFLAGS) runner.o recorder.o network.o datareceiver.o h5filerecorder.o tspiketable.o recorder_test.o -o recorder_runner 
clean:
	rm *.o

