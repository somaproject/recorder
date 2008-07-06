"""
The "noteable" interface, which lets you append notes
to objects.

There are two simple methods:


GetNotes() returns an array of note structs
AppendNote(note) appends the note struct Note to the current Notes

Note Struct consists of:
   username
   datetime (filled in by server)
   timestamp (filled in by server)
   text in markup form
   

"""

