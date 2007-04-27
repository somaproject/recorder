#include <iostream>
#include <string>
#include <vector>
#include <sys/time.h>
#include <time.h>
#include <boost/format.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>

using std::cout;
using std::endl;

#include "H5Cpp.h"
#include "hdf5_hl.h"

using namespace H5;



#include "tspike.h" 

using boost::format;

hid_t file_id; 
const int NFIELDS = 3; 
size_t TSpike_dst_size =  sizeof( TSpike_t );
size_t TSpike_dst_offset[NFIELDS] = { HOFFSET( TSpike_t, src ),
					HOFFSET( TSpike_t, time ),
					HOFFSET(TSpike_t, waveforms)}; 
TSpike_t tests; 
size_t TSpike_dst_sizes[NFIELDS] = { sizeof( tests.src), 
					   sizeof( tests.time),
					   sizeof( tests.waveforms)}; 

int appendRecords(const std::vector<TSpike_t> & spikes,
		  int CSIZE, int start, int end); 

int tabletest(int num, int blocksize, int chunksize, std::string filename)
{

  

  hsize_t wave_dims[1] = {TSPIKEWAVE_LEN}; 
  hid_t wt = H5Tarray_create(H5T_NATIVE_INT32, 1, wave_dims, NULL);  

  hid_t TSpikeWave_type = H5Tcreate(H5T_COMPOUND, sizeof(TSpikeWave_t)); 
  H5Tinsert(TSpikeWave_type, "filtid", 
	    HOFFSET(TSpikeWave_t, filtid), H5T_NATIVE_UINT8); 
  H5Tinsert(TSpikeWave_type, "valid", 
	    HOFFSET(TSpikeWave_t, valid), H5T_NATIVE_UINT8); 
  H5Tinsert(TSpikeWave_type, "threshold", 
	    HOFFSET(TSpikeWave_t, threshold), H5T_NATIVE_INT32); 
  H5Tinsert(TSpikeWave_type, "wave", 
	    HOFFSET(TSpikeWave_t, wave), wt); 
  hsize_t TSpikeWave_type_dims[1] = {4}; 
  hid_t waveforms_array_t = H5Tarray_create(TSpikeWave_type, 
					    1, TSpikeWave_type_dims,
					    NULL);  
  
  
  
  
  const char *TSpike_field_names[NFIELDS]  =
    { "src", "time", "waveforms"}; 
  
  hid_t TSpike_field_type[NFIELDS]; 
  TSpike_field_type[0] = H5T_NATIVE_UINT8; 
  TSpike_field_type[1] = H5T_NATIVE_UINT64; 
  TSpike_field_type[2] = waveforms_array_t; 


  hsize_t    chunk_size = chunksize;
  int        *fill_data = NULL;
  int        compress  = 0;
  herr_t     status;
  int        i;

  
  
  file_id = H5Fcreate(filename.c_str(),  H5F_ACC_TRUNC, 
			      H5P_DEFAULT, H5P_DEFAULT );
  
  
  status = H5TBmake_table( "Table Title", file_id, "TESTTABLE",
			   NFIELDS, 0,
			   TSpike_dst_size,
			   TSpike_field_names, 
			   TSpike_dst_offset, 
			   TSpike_field_type,
			   1, NULL, false, NULL  );
  
  // build up giant-ass array of data
  int N = num; 
  int CSIZE = blocksize; 
  std::vector<TSpike_t> spikes(N); 
  for (int i = 0; i < N; i++) {
    spikes[i].src = 1; 
    spikes[i].time = i; 
    for (int j = 0; j < 4; j++) {
      spikes[i].waveforms[j].filtid = j; 
      spikes[i].waveforms[j].valid = true; 
      spikes[i].waveforms[j].threshold = i + j*4 ; 
      for (int k = 0; k < TSPIKEWAVE_LEN; k++) {
	spikes[i].waveforms[j].wave[k] = i*100 + j*4 + k; 
      }
    }
  }

  timeval t1, t2; 
  gettimeofday(&t1, NULL); 

  int mc = N/CSIZE; 
//   boost::thread thrd1(boost::bind(&appendRecords, spikes, CSIZE, 0, mc/2)); 
//   boost::thread thrd2(boost::bind(&appendRecords, spikes, CSIZE, mc/2, mc)); 
  appendRecords(spikes, CSIZE, 0, mc); 

//   thrd1.join(); 
//   thrd2.join(); 
  gettimeofday(&t2, NULL); 
  
  int usecs = (t2.tv_sec - t1.tv_sec)*1000000 + (t2.tv_usec - t1.tv_usec); 
  H5Fclose(file_id); 
  return usecs; 

}

int appendRecords(const std::vector<TSpike_t> & spikes,
		  int CSIZE, int start, int end)
{
  int N = spikes.size(); 

  for (int i = start; i < end; i++)
    {
      H5TBappend_records(file_id, "TESTTABLE", CSIZE, 
			 sizeof(TSpike_t), 
			 TSpike_dst_offset, 
			 TSpike_dst_sizes, 
			 &spikes[i * CSIZE]); 
    }

}

int main(int argc, char * argv[])
{

//   po::options_description desc("Allowed options");
//   desc.add_options()
//     ("help", "produce help message")
//     ("num", po::value<int>(), "number of records")
//     ("blocksize", po::value<int>(), "number to attempt to write at once")
//     ("chunksize", po::value<int>(), "hdf5 chunksize")
//     ("filename", po::value<int>(), "filename to write");


//   po::variables_map vm;
//   po::store(po::parse_command_line(ac, av, desc), vm);
//   po::notify(vm);    

  std::cout << "num    csize    bsize   time" << std::endl; 
  int num[4] = {100000, 500000, 1000000, 5000000}; 
  int csize[5] = {10, 50, 100, 500, 1000}; 
  int bsize[5] = {10, 50, 100, 500, 1000}; 
  for (int inum = 0; inum  < 4; inum++) {
    for (int icsize =0 ; icsize < 5; icsize++) {
      for (int ibsize = 0; ibsize < 5; ibsize++) {
	std::cout << num[inum] << " " 
		  << csize[icsize] << " " 
		  << bsize[ibsize] << " "; 
	//std::string fname = boost::str(boost::format("test.%1%.%2%.%3%.h5") % num[inum] % csize[icsize] % bsize[ibsize]); 
	std::string fname("/mnt/tmpfs/test.h5"); 
	int time = tabletest(num[inum], bsize[ibsize], csize[icsize], fname);
	std::cout << time << std::endl; 

      }
    }
  }
}
