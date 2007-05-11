#ifndef TSPIKE_TYPE_H
#define TSPIKE_TYPE_H

#include <stdint.h>

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
  TSpikeWave_t x; 
  TSpikeWave_t y; 
  TSpikeWave_t a; 
  TSpikeWave_t b; 

}; 



#endif // TSPIKE_H_H
