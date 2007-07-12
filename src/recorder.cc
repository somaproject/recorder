#include "recorder.h"
#include <boost/format.hpp>
static const char* RECORD_NAME = "org.soma.Recorder";
static const char* RECORD_PATH = "/org/soma/Recorder";

using namespace soma::recorder; 
using boost::format; 
void logMessage(std::string str)
{
  std::cout << str << std::endl; 
}

Recorder::Recorder(DBus::Connection & connection) :
  DBus::ObjectAdaptor(connection, RECORD_PATH), 
  recording_(false)
{
  logMessage("Recorder Created"); 
  
}

void Recorder::CreateFile(const DBus::String & filename)
{
  logMessage("createFile called with" + filename); 

  pH5F_.reset(new H5FileRecorder(filename)); 

}

void Recorder::CloseFile()
{
  
  checkRecording(false); 
  
  pH5F_.reset(); 
  logMessage("closeFile called"); 
  //Gtk::Main::quit(); 
}

void Recorder::CreateEpoch(const DBus::String & epochname)
{
  checkRecording(false); 

  pH5F_->createEpoch(epochname); 
  
}

void Recorder::SwitchEpoch(const DBus::String & epochname)
{
  checkRecording(false); 

  pH5F_->switchEpoch(epochname); 
  // get all the existing epochs in that 
  
}

DBus::String Recorder::GetEpoch()
{
  checkRecording(false); 

  return DBus::String("Hello world"); 
  
}

void Recorder::EnableDataRX(const DBus::Int32 & src, const DBus::Int32 & typ)
{
  checkRecording(false); 

  datatype_t dtyp = charToDatatype(char(typ)); 
  pH5F_->enableRX(src, dtyp); 

}

void Recorder::DisableDataRX(const DBus::Int32 & src, const DBus::Int32 & typ)
{
  checkRecording(false); 

  datatype_t dtyp = charToDatatype(char(typ)); 

  pH5F_->disableRX(src, dtyp); 

}

void Recorder::StartRecording()
{
  checkRecording(false); 

  // create the network object
  pNetwork_.reset(new Network("127.0.0.1")); 
  // enable the relevant data bits
  std::list<dpair_t> datarxlist =   pH5F_->getDataRX(); 
  for (std::list<dpair_t>::iterator i = datarxlist.begin(); 
       i != datarxlist.end(); i++) 
    {
    
      std::cout << " We are configured to receive " 
		<< int(i->first) << std::endl; 
      pNetwork_->enableDataRX(i->first, i->second); 
    }

  // hook up the callback pipe
  
  sigNewData_ = 
    Glib::signal_io().connect(sigc::mem_fun(this, &Recorder::newDataCallback),
			      pNetwork_->getDataFifoPipe(), 
			      Glib::IO_IN);
  
  sigNewEvent_ = 
    Glib::signal_io().connect(sigc::mem_fun(this, &Recorder::newEventCallback),
			      pNetwork_->getEventFifoPipe(), 
			      Glib::IO_IN);
  
  // start the thread
  recording_ = true; 
  pNetwork_->run(); 
  
}

void Recorder::StopRecording()
{
  
  checkRecording(true); 
  std::cout << "Stopping recording..."; 
  // disconnect the pipe
  sigNewData_.disconnect(); 
  
  // delete the object
  delete pNetwork_.release(); 
  recording_ = false; 
  std::cout << " done" << std::endl;
}

std::map< DBus::String, DBus::String> Recorder::NetworkStats()
{
  checkRecording(true); 
  
  std::vector<DataReceiverStats> drstats =   pNetwork_->getDataStats(); 
  
  std::map< DBus::String, DBus::String > info;
  std::vector<DataReceiverStats>::iterator i; 
  for (i = drstats.begin(); i != drstats.end(); i++)
    {
      std::string key = str(format("src=%1% typ=%2%") % i->source % i->type ); 
      std::string value = str(format("pktCount=%1%, latestSeq=%2%, dupeCount=%3%, pendingCount=%4%, missingPacketCount=%5%, reTxRxCount=%6%, outOfOrderCount=%7%") % i->pktCount % i->latestSeq % i->dupeCount % i->pendingCount % i->missingPacketCount % i->reTxRxCount % i->outOfOrderCount); 

      info[key] = value; 
    }

  return info; 

}


bool Recorder::newDataCallback(Glib::IOCondition io_condition)
{
  if (io_condition & Glib::IO_IN) {
    char x; 
    read(pNetwork_->getDataFifoPipe(), &x, 1); 
    
    DataPacket_t * rdp = pNetwork_->getNewData(); 
    pH5F_->append(rdp); 
    delete rdp;
  }
  return true; 
}

bool Recorder::newEventCallback(Glib::IOCondition io_condition)
{
  if (io_condition & Glib::IO_IN) {
    char x; 
    read(pNetwork_->getEventFifoPipe(), &x, 1); 
    
    DataPacket_t * rdp = pNetwork_->getNewData(); 
    pH5F_->append(rdp); 
    delete rdp;
  }
  return true; 
}

void Recorder::checkRecording(bool isRecording)
{
  if (isRecording == true and recording_ == false)
    {
      throw std::logic_error("Not currently recording"); 
    }
  if (isRecording == false and recording_ == true)
    {
      throw std::logic_error("Currently recording"); 
    }

}
