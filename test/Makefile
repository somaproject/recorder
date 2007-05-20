CXXFLAGS = -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D_POSIX_SOURCE -D_BSD_SOURCE   -O2 -D_REENTRANT -I../src/

LDFLAGS = -lhdf5 -lhdf5_cpp -lhdf5_hl -lboost_thread 

#all: compound 

#compound.o: compound.cc


h5filerecorderbench: h5filerecorderbench.o ../src/h5filerecorder.o ../src/tspiketable.o

compound: compound.o

clean: 
	rm *.o
