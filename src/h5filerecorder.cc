#include <iostream>
#include <sys/time.h>
#include <time.h>
#include "h5filerecorder.h"
#include <hdf5.h>
#include "tspiketable.h"
using namespace soma::recorder; 


H5FileRecorder::H5FileRecorder(const std::string  & filename) :
  filename_(filename),
  h5file_(filename, H5F_ACC_TRUNC),
  epochGroup_()
{
  //H5::Exception::dontPrint(); 
}

H5FileRecorder::~H5FileRecorder()
{
  h5file_.flush(H5F_SCOPE_GLOBAL); 
  h5file_.close(); 

}

void H5FileRecorder::createEpoch(const std::string & epochName)
{
 
  H5::Group newGroup =  h5file_.createGroup(epochName);

  H5::DataSpace attrspace; 
  
  H5::Attribute createtime = newGroup.createAttribute("createtime", 
						      H5::PredType::NATIVE_LLONG,
						      attrspace); 
  
  timeval tval; 
  gettimeofday(&tval, NULL); 
  long long secSinceEpoch = tval.tv_sec; 
  
  createtime.write(H5::PredType::NATIVE_LLONG, 
		   &secSinceEpoch); 

}

void H5FileRecorder::switchEpoch(const std::string & epochName)
{
  
  epochGroup_ = h5file_.openGroup(epochName);
  
  // close existing dispatch table:
  for (dispatchTable_t::iterator i = dispatchTable_.begin();
       i != dispatchTable_.end(); i++) 
    {
      dpair_t ds = i->first;
      datatype_t typ = ds.second; 
      datasource_t src = ds.first; 
      disableDataRX( src, typ); 
      
    }
}

H5::Group H5FileRecorder::getTypeGroup(datatype_t typ)
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
    std::cout << "Creating group ..." << typeName << std::endl; 
    hg =  epochGroup_.createGroup(typeName); 
  }
  
  return hg; 
}

void H5FileRecorder::enableDataRX(datasource_t src, std::string name, 
				  datatype_t typ)
{
  H5::Group hg = getTypeGroup(typ); 
  
  DatasetIO * pIO; 
  switch (typ) {
  case TSPIKE:
    pIO = new TSpikeTable(src, name, hg);
    break; 
  case WAVE: 
    pIO = new TSpikeTable(src, name, hg);
    break; 
  default: 
    throw std::runtime_error("Do not know how to save that type of data");
  }

  dpair_t dataorigin(src, typ); 
  dispatchTable_[dataorigin] = pIO; 
  
}

void H5FileRecorder::disableDataRX(datasource_t src, datatype_t typ)
{

  dpair_t dataorigin(src, typ); 
  DatasetIO * tst; 
  tst = dispatchTable_[dataorigin]; 
  delete tst; 
  dispatchTable_.erase(dispatchTable_.find(dataorigin)); 
  
}

void H5FileRecorder::appendData(pDataPacket_t rdp)
{
  // make sure we really want this data
   datasource_t src = rdp->src; 
 
   datatype_t typ = rdp->typ; 

   dpair_t dataorigin(src, typ); 
  
   dispatchTable_[dataorigin]->append(rdp); 

}

std::list<dpair_t> H5FileRecorder::getDataRX()
{
  std::list<dpair_t> datarxlist; 
  dispatchTable_t::iterator i; 
  for (i = dispatchTable_.begin(); i != dispatchTable_.end();
       i++)
    {
      
      datarxlist.push_back(i->first); 
    }
  return datarxlist; 
    

}
