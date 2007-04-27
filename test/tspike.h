#ifndef TSPIKE_TYPE_H
#define TSPIKE_TYPE_H


const int TSPIKEWAVE_LEN = 32; 

struct TSpikeWave_t {
  uint8_t filtid; 
  uint8_t valid; 
  int32_t threshold; 
  int32_t wave[TSPIKEWAVE_LEN]; 
}; 

struct TSpike_t
{
  uint8_t src; 
  uint64_t time; 
  TSpikeWave_t waveforms[4]; 
}; 


#endif // TSPIKE_H_H
