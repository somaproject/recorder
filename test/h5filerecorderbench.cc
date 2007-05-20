#include <h5filerecorder.h>
#include <iostream>
#include <string>
#include <vector>
#include <sys/time.h>
#include <time.h>

int main()
{
  
  soma::recorder::H5FileRecorder * h5f; 
  h5f = new soma::recorder::H5FileRecorder("/mnt/tmpfs/testing.h5"); 
  // create 64 endpoints
  h5f->createEpoch("test"); 
  h5f->switchEpoch("test"); 
  const int SRCN = 64; 
  for (int i = 0; i < SRCN; i++)
    {
      h5f->enableRX(i, TSPIKE); 
    }

  timeval t1, t2; 
  gettimeofday(&t1, NULL); 

  for (int i = 0; i < 10000; i++)
    {
      for (int n = 0; n < SRCN; n++) 
	{
	  RawData x; 
	  x.src = n; 
	  x.typ = TSPIKE; 
	  x.seq = i; 
	  h5f->append(&x); 
	}
    }
  delete h5f; 
  gettimeofday(&t2, NULL); 
  
  int usecs = (t2.tv_sec - t1.tv_sec)*1000000 + (t2.tv_usec - t1.tv_usec); 
  std::cout << usecs << std::endl; 

}

