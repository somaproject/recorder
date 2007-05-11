
SRCDIR = ../src/
CXXFLAGS=-I$(SRCDIR) -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D_POSIX_SOURCE -UNDEBUG
LDFLAGS=-lboost_unit_test_framework  -lhdf5_cpp -lhdf5  -lboost_filesystem -lhdf5_hl

all: runner


tspiketable.o: $(SRCDIR)/tspiketable.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/tspiketable.cc


h5filerecorder.o: $(SRCDIR)/h5filerecorder.cc
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/h5filerecorder.cc

H5FileRecorder_create.o: h5filerecorder.o

runner: runner.o H5FileRecorder_create.o h5filerecorder.o tspiketable.o

clean:
	rm *.o

