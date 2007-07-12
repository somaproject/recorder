CXXFLAGS=-I$(SRCDIR) -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D_POSIX_SOURCE -UNDEBUG -I./ -g `pkg-config --cflags dbus-c++-1 glibmm-2.4 gtkmm-2.4 gdkmm-2.4 glibmm-2.4 somanetwork-1.0` -O3 -pthread

LDFLAGS= -lboost_thread  -lhdf5_cpp -lhdf5  -lboost_filesystem -lhdf5_hl `pkg-config --libs dbus-c++-1 gtkmm-2.4 gdkmm-2.4 glibmm-2.4 somanetwork-1.0`

all: recorder

recorder-glue.h: recorder-introspect.xml
	/usr/local/bin/dbusxx-xml2cpp recorder-introspect.xml --adaptor=recorder-glue.h

recorder.o: recorder-glue.h

recorder: recordermain.o recorder.o tspiketable.o h5filerecorder.o 

clean:
	rm *.o
