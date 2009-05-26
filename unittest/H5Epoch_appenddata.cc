#include <boost/test/auto_unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <iostream>                        
#include <H5Cpp.h>
#include <tspiketable.h>
#include "wavetable.h"
#include <somanetwork/fakenetwork.h>
#include "h5epoch.h"

using namespace soma; 
using namespace boost;       
using namespace boost::filesystem; 

BOOST_AUTO_TEST_SUITE(H5Epoch_appenddata); 

BOOST_AUTO_TEST_CASE(H5Epoch_appenddata1)
{
  // we enable and disable a bunch of data sources and then 
  // check to see if we can get them all 

  // CREATE FILE
  {
    std::string filename = "H5Epoch_appenddata1.h5"; 
    filesystem::remove_all(filename); 
    
    // separate block to get delete called on object
    pNetworkInterface_t pfn(new FakeNetwork()); 
    
    recorder::pExperiment_t pExp =
      recorder::H5Experiment::create(pfn, filename); 
    
    recorder::pEpoch_t ei  = pExp->createEpoch("myEpoch"); 
    
    somanetwork::datasource_t TSRC = 23;   
    somanetwork::datasource_t WSRC = 7; 
    ei->enableDataSink(TSRC, TSPIKE); 
    ei->setDataName(TSRC, "hipp1"); 
    
    
    ei->enableDataSink(WSRC, WAVE); 
    ei->setDataName(WSRC, "hipp4"); 
    
    ei->startRecording(); 
    // ADD THE DATA
    
    int N = 100; 
    
    for (int i = 0; i < N; i++)
      {
	TSpike_t ts; 
	ts.src = TSRC; 
	ts.time = i; 
	TSpikeWave_t * tswp[4]; 
	tswp[0] = &ts.x; 
	tswp[1] = &ts.y; 
	tswp[2] = &ts.a; 
	tswp[3] = &ts.b; 
	
	for (int j = 0; j < 4; j++) {
	  TSpikeWave_t * tw = tswp[j]; 
	  tw->filtid = i * 5 + j; 
	  tw->valid = 1; 
	  tw->threshold = i * 0x1234 + j * 0xAB; 
	  for (int k = 0; k < TSPIKEWAVE_LEN; k++) {
	    tw->wave[k] = i + j + k; 
	  }
	}
	
	pDataPacket_t rdp(rawFromTSpike(ts)); 
	ei->appendData(rdp); 
      }
    
    N = 200; 
    uint64_t offsetval; 
    offsetval = 0x01234567; 
    offsetval = offsetval << 16; 
    offsetval |= 0x89AB; 
    for (uint64_t i = 0; i < N; i++)
      {
	Wave_t w; 
	w.src = WSRC; 
	w.time = i * offsetval;
	w.sampratenum = 0x789A; 
	w.samprateden = 0x1234; 
	w.selchan = 0x1234; 
	w.filterid = 0xAABB; 
      
	for (int j = 0; j < WAVEBUF_LEN; j++) {
	  w.wave[j] = i * 0xABCD + j*0x17; 
	}
	
	pDataPacket_t rdp = rawFromWave(w); 
	ei->appendData(rdp); 
	
	
      }
    ei->stopRecording(); 

    // now we need to flush the file
  }
  int retval = std::system("python H5Epoch_appenddata.py appenddata1");
  BOOST_CHECK_EQUAL(retval , 0); 

}


BOOST_AUTO_TEST_SUITE_END(); 

// BOOST_AUTO_TEST_SUITE(H5Epoch_notetest); 

// BOOST_AUTO_TEST_CASE(H5Epoch_notetest_apitest)
// {
//   // Test the note create / remove / etc. interfaces

//   // create a temp file
//   H5::H5File::H5File * h5file 
//     = new H5::H5File::H5File("H5Epoch_notetest_apitest.h5", H5F_ACC_TRUNC); 

//   H5::Group grp = h5file->createGroup("testGroup");
  
//   recorder::H5Epoch epoch(grp, "TestEpoch"); 
//   h5file->flush(H5F_SCOPE_GLOBAL); 

//   epoch.createNote("test1", "This is our test note 1"); 
//   epoch.createNote("test2", "This is our test note 2"); 

//   BOOST_CHECK_THROW(epoch.createNote("test1", "This is our test note 1"), 
// 		    std::runtime_error); 

//   // ----------------------------------------------------------
//   // check if we throw an error when we try to write too long of a note
//   // ----------------------------------------------------------
//   std::string lname = ""; 
//   for (int i = 0; i < NAMELEN + 1; i++) {
//     lname += "a"; 
//   }; 
//   BOOST_CHECK_THROW(epoch.createNote(lname, "Too long of a name"), 
// 		    std::runtime_error); 
  
//   // ----------------------------------------------------------
//   // check if we can read notes
//   // ----------------------------------------------------------
  
//   Note_t notecheck = epoch.getNote("test2"); 
//   BOOST_CHECK_EQUAL(notecheck.text, "This is our test note 2"); 

//   notecheck = epoch.getNote("test1"); 
//   BOOST_CHECK_EQUAL(notecheck.text, "This is our test note 1"); 
  
//   // ----------------------------------------------------------
//   // check if we can write notes
//   // ----------------------------------------------------------
//   epoch.setNote("test2", "I am not a cylon!"); 
//   notecheck = epoch.getNote("test2"); 
//   BOOST_CHECK_EQUAL(notecheck.text, "I am not a cylon!"); 
  
//   // ----------------------------------------------------------
//   // check if we can delete notes
//   // ----------------------------------------------------------
//   epoch.deleteNote("test2");
//   BOOST_CHECK_THROW(epoch.getNote("test2"), std::runtime_error); 
  
//   // ----------------------------------------------------------
//   // check if we can get all the notes
//   // ----------------------------------------------------------
//   epoch.createNote("Test3", "Still not a cylon."); 
//   std::list<Note_t> notes = epoch.getAllNotes();
//   BOOST_CHECK_EQUAL(notes.size(), 2); 

//   h5file->close(); 
//   delete h5file; 

// //   int retval = system("python NoteTable_test.py create");
// //  BOOST_CHECK_EQUAL(retval , 0); 

// }


// BOOST_AUTO_TEST_CASE(H5Epoch_notetest_diskcheck)
// {
//   // Test the note create / remove / etc. interfaces

//   // create a temp file
//   H5::H5File::H5File * h5file 
//     = new H5::H5File::H5File("H5Epoch_notetest_diskcheck.h5", H5F_ACC_TRUNC); 

//   H5::Group grp = h5file->createGroup("testGroup");
  
//   recorder::H5Epoch * epoch = new recorder::H5Epoch(grp, "TestEpoch"); 
//   h5file->flush(H5F_SCOPE_GLOBAL); 

//   epoch->createNote("test1", "This is our test note 1"); 
//   epoch->createNote("test2", "This is our test note 2"); 

//   // the third note tests the edge cases
//   std::string longname = "";
//   for (int i = 0; i < NAMELEN; i++) {
//     char c = 65 + (i % 26); 
//     longname += c; 
//   }

//   std::string longtext = "";
//   for (int i = 0; i < NOTELEN; i++) {
//     char c = 65 + (i % 26); 
//     longtext += c; 
//   }
//   epoch->createNote(longname, longtext); 

//   // ----------------------------------------------------------
//   // write to disk by deleting
//   // ----------------------------------------------------------
//   delete epoch; 
//   h5file->flush(H5F_SCOPE_GLOBAL); 

//   h5file->close(); 
//   delete h5file; 

//   int retval = system("python H5Epoch_appenddata.py notecheck");
//   BOOST_CHECK_EQUAL(retval , 0); 

  

// }
// BOOST_AUTO_TEST_SUITE_END(); 
