import gobject
import gtk

class NotesGUI(object):

    def __init__(self, notesvbox, addbutton, getfunc, createfunc):

        self.vboxNotes = notesvbox
        self.buttonAddNote = addbutton

        self.getfunc = getfunc
        self.createfunc = createfunc

        self.buttonAddNote.connect('clicked', self.on_buttonAddNote_clicked)
        

    def on_buttonAddNote_clicked(self, widget):
        """
        Clicked when the "Add note" button is pressed

        Add note should set things up such that
          1. you can't add another note until this one is cancelled or saved
          
          2. when you save it, it is pushed ot the server and then we
             grab all notes
             a. clear all the notes
             b. put up a change dialog box
             c. show all the notes
     

        """
        print "adding note"
        
        frame = gtk.Frame()
        entry = gtk.Entry()
        hbox = gtk.HBox()
        hbox.pack_start(gtk.Label("Title:"))
        hbox.pack_start(entry)
                        
        frame.set_label_widget(hbox)
        textbuffer = gtk.TextBuffer(table=None)
        
        textview = gtk.TextView(textbuffer)
        textview.set_wrap_mode(gtk.WRAP_WORD)
        vbox = gtk.VBox()
        vbox.pack_start(textview)
        
        buttonbox = gtk.HButtonBox()
        buttonbox.set_layout(gtk.BUTTONBOX_END)

        savebutton = gtk.Button(label="Save", stock=gtk.STOCK_SAVE)
        savebutton.connect('clicked', self.noteSave, frame, entry, textbuffer)
        
        cancelbutton = gtk.Button(label="Cancel", stock=gtk.STOCK_CANCEL)
        cancelbutton.connect('clicked', self.noteCancel, frame, entry, textbuffer)
        
        buttonbox.add(savebutton)
        buttonbox.add(cancelbutton)
        vbox.pack_start(buttonbox)

        frame.add(vbox)
        
        self.vboxNotes.pack_start(frame)
        self.buttonAddNote.props.sensitive = False
        self.vboxNotes.show_all()

    def noteSave(self, widget, expander, entry, textbuffer):
        self.createfunc(entry.props.text, textbuffer.props.text)
        expander.destroy()
        self.buttonAddNote.props.sensitive = True
        self.updateNotes()

    def noteCancel(self, widget, expander, entry, textbuffer):
        print "Note cancel"
        expander.destroy()


        self.buttonAddNote.props.sensitive = True

    def preventExpand(self, expander):
        expander.props.expanded = False
    
    def setNotesPending(self):
        """
        called to make the notes pending

        """
        self.vboxNotes.hide()

        # set the thing to be pending
        
        
    def updateNotes(self):
        """
        called to update the list of notes
        """
        notes = self.getfunc()
        for i in self.vboxNotes:
            self.vboxNotes.remove(i)
            
        
        for (ts, time, title, text) in notes:
            expander = gtk.Expander()
            label = gtk.Label(title)
            expander.set_label_widget(label)
            window = gtk.ScrolledWindow()
            window.props.hscrollbar_policy=gtk.POLICY_NEVER
            window.props.vscrollbar_policy=gtk.POLICY_AUTOMATIC
            window.props.shadow_type = gtk.SHADOW_IN
            
            textbuffer = gtk.TextBuffer(table=None)
            
            textbuffer.set_text(text)
            
            textview = gtk.TextView(textbuffer)
            expander.set_expanded(True)

            textview.set_editable(False)
            textview.props.editable = False
            textview.props.cursor_visible = False
            textview.set_wrap_mode(gtk.WRAP_WORD)
            window.add(textview)
            expander.add(window)
            self.vboxNotes.pack_start(expander)
        
        self.vboxNotes.show_all()
