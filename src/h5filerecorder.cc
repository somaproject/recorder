#include "h5filerecorder.h"
#include <iostream>
#include "hdf5.h"
#include "tspiketable.h"
using namespace soma::recorder; 


H5FileRecorder::H5FileRecorder(const std::string  & filename) :
  filename_(filename),
  h5file_(filename, H5F_ACC_TRUNC),
  epochGroup_()
{

  H5::Exception::dontPrint(); 
}

H5FileRecorder::~H5FileRecorder()
{


}

void H5FileRecorder::createEpoch(const std::string & epochName)
{
  h5file_.createGroup(epochName);

}

void H5FileRecorder::switchEpoch(const std::string & epochName)
{
  try {
    epochGroup_ = h5file_.openGroup(epochName);

    // close existing dispatch table:
    for (dispatchTable_t::iterator i = dispatchTable_.begin();
	 i != dispatchTable_.end(); i++) 
      {
	datasrc_t ds = i->first;
	DATATYPES typ = ds.first;
	int src = ds.second; 
	disableRX(typ, src); 

      }
  } 
  catch(H5::FileIException & e){ 
    std::cout << e.getCDetailMsg() << std::endl; 
    
  }

}

H5::Group H5FileRecorder::getTypeGroup(DATATYPES typ)
{
  // get the group for the type if it exists
  std::string typeName;
  switch(typ) 
    {
    case TSPIKE:
      typeName = "tspikes"; 
      break; 
    case WAVE:
      typeName = "waves"; 
      break; 
    case RAW:
      typeName = "raw"; 
      break; 
    }
  
  H5::Group hg; 

  try 
    {
      hg =  epochGroup_.openGroup(typeName); 

    } 
  catch(H5::GroupIException & e){ 
    hg =  epochGroup_.createGroup(typeName); 
  }

  return hg; 
}

void H5FileRecorder::enableRX(DATATYPES typ, int src)
{

  H5::Group hg = getTypeGroup(typ); 
  TSpikeTable *  tst = new TSpikeTable(src, hg);
  datasrc_t dataorigin(typ, src); 
  dispatchTable_[dataorigin] = (DatasetIO *)tst; 
  
}

void H5FileRecorder::disableRX(DATATYPES typ, int src)
{

  datasrc_t dataorigin(typ, src); 
  DatasetIO * tst; 
  tst = dispatchTable_[dataorigin]; 
  delete tst; 
  dispatchTable_.erase(dispatchTable_.find(dataorigin)); 
  
  
  
}


