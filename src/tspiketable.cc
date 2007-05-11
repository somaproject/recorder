
#include "tspike.h"
#include "tspiketable.h"
#include <boost/format.hpp> 
#include <iostream>

void createTSpikeTable(int src, hid_t loc)
{

  const int NFIELDS = 3; 
  size_t TSpike_dst_size =  sizeof( TSpike_t );
  size_t TSpike_dst_offset[NFIELDS] = { HOFFSET( TSpike_t, src ),
					HOFFSET( TSpike_t, time ),
					HOFFSET(TSpike_t, waveforms)}; 
  TSpike_t tests; 
  size_t TSpike_dst_sizes[NFIELDS] = { sizeof( tests.src), 
				       sizeof( tests.time),
				       sizeof( tests.waveforms)}; 
  
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
  hsize_t TSpikeWave_type_dims[] = {4}; 
  hid_t waveforms_array_t = H5Tarray_create(TSpikeWave_type, 
 					    1, TSpikeWave_type_dims,
 					    NULL);  
  
  
  
  
  const char *TSpike_field_names[NFIELDS]  =
    { "src", "time", "waveforms"}; 
  
  hid_t TSpike_field_type[NFIELDS]; 
  TSpike_field_type[0] = H5T_NATIVE_UINT8; 
  TSpike_field_type[1] = H5T_NATIVE_UINT64; 
  TSpike_field_type[2] = waveforms_array_t; 


  int        *fill_data = NULL;
  int        compress  = 0;
  herr_t     status;
  int        i;

  std::cout << "The data structure is " << TSpike_dst_size << " bytes" << std::endl; 
  std::string tablename = str(boost::format("chan%1d") % src); 
  
  status = H5TBmake_table( "Table Title", loc, tablename.c_str(), 
			   NFIELDS, 0,
			   TSpike_dst_size,
			   TSpike_field_names, 
			   TSpike_dst_offset, 
			   TSpike_field_type,
			   1, NULL, false, NULL  );
  
}
