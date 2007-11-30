
#include <boost/test/auto_unit_test.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"   
#include <iostream>                        
#include <fstream>
#include "notetable.h"
using namespace soma; 
using  namespace boost;       
using namespace boost::filesystem; 
using namespace std; 

BOOST_AUTO_TEST_SUITE(NoteTableTest); 

BOOST_AUTO_TEST_CASE(NoteTable_create)
{

  // create a temp file
  H5::H5File::H5File * h5file 
    = new H5::H5File::H5File("NoteTable_create.h5", H5F_ACC_TRUNC); 

  H5::Group grp = h5file->createGroup("testGroup");
  
  recorder::NoteTable tst(grp); 
  h5file->flush(H5F_SCOPE_GLOBAL); 

  h5file->close(); 
  delete h5file; 

//   int retval = system("python NoteTable_test.py create");
//  BOOST_CHECK_EQUAL(retval , 0); 

}

BOOST_AUTO_TEST_CASE(NoteTable_apitest)
{
  // Test the note create / remove / etc. interfaces

  // create a temp file
  H5::H5File::H5File * h5file 
    = new H5::H5File::H5File("NoteTable_apitest.h5", H5F_ACC_TRUNC); 

  H5::Group grp = h5file->createGroup("testGroup");
  
  recorder::NoteTable notetable(grp); 
  h5file->flush(H5F_SCOPE_GLOBAL); 

  notetable.createNote("test1", "This is our test note 1"); 
  notetable.createNote("test2", "This is our test note 2"); 

  BOOST_CHECK_THROW(notetable.createNote("test1", "This is our test note 1"), 
		    std::runtime_error); 

  // ----------------------------------------------------------
  // check if we throw an error when we try to write too long of a note
  // ----------------------------------------------------------
  std::string lname = ""; 
  for (int i = 0; i < NAMELEN + 1; i++) {
    lname += "a"; 
  }; 
  BOOST_CHECK_THROW(notetable.createNote(lname, "Too long of a name"), 
		    std::runtime_error); 

  // ----------------------------------------------------------
  // check if we can read notes
  // ----------------------------------------------------------

  Note_t notecheck = notetable.getNote("test2"); 
  BOOST_CHECK_EQUAL(notecheck.text, "This is our test note 2"); 

  notecheck = notetable.getNote("test1"); 
  BOOST_CHECK_EQUAL(notecheck.text, "This is our test note 1"); 
  
  // ----------------------------------------------------------
  // check if we can write notes
  // ----------------------------------------------------------
  notetable.setNote("test2", "I am not a cylon!"); 
  notecheck = notetable.getNote("test2"); 
  BOOST_CHECK_EQUAL(notecheck.text, "I am not a cylon!"); 
  
  // ----------------------------------------------------------
  // check if we can delete notes
  // ----------------------------------------------------------
  notetable.deleteNote("test2");
  BOOST_CHECK_THROW(notetable.getNote("test2"), std::runtime_error); 
  
  // ----------------------------------------------------------
  // check if we can get all the notes
  // ----------------------------------------------------------
  notetable.createNote("Test3", "Still not a cylon."); 
  std::list<Note_t> notes = notetable.getAllNotes();
  BOOST_CHECK_EQUAL(notes.size(), 2); 

  h5file->close(); 
  delete h5file; 

//   int retval = system("python NoteTable_test.py create");
//  BOOST_CHECK_EQUAL(retval , 0); 

}


BOOST_AUTO_TEST_CASE(NoteTable_diskcheck)
{
  // Test the note create / remove / etc. interfaces

  // create a temp file
  H5::H5File::H5File * h5file 
    = new H5::H5File::H5File("NoteTable_diskcheck.h5", H5F_ACC_TRUNC); 

  H5::Group grp = h5file->createGroup("testGroup");
  
  recorder::NoteTable * notetable = new recorder::NoteTable(grp); 
  h5file->flush(H5F_SCOPE_GLOBAL); 

  notetable->createNote("test1", "This is our test note 1"); 
  notetable->createNote("test2", "This is our test note 2"); 

  // the third note tests the edge cases
  std::string longname = "";
  for (int i = 0; i < NAMELEN; i++) {
    char c = 65 + (i % 26); 
    longname += c; 
  }

  std::string longtext = "";
  for (int i = 0; i < NOTELEN; i++) {
    char c = 65 + (i % 26); 
    longtext += c; 
  }
  notetable->createNote(longname, longtext); 

  // ----------------------------------------------------------
  // write to disk by deleting
  // ----------------------------------------------------------
  delete notetable; 
  h5file->flush(H5F_SCOPE_GLOBAL); 

  h5file->close(); 
  delete h5file; 

  int retval = system("python NoteTable_test.py diskcheck");
  BOOST_CHECK_EQUAL(retval , 0); 

  

}
BOOST_AUTO_TEST_SUITE_END(); 
