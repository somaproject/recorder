#!/usr/bin/env python

import sys
import pygtk
pygtk.require("2.0")
import gtk
import gtk.glade
import pdb
from epoch import Epoch
from recorder import Recorder
from treemodel import RecorderTreeModel
import datatab
import notesgui

class EpochProperty(object):
    """
    This is the class responsible for managing the epoch properties,
    recording, etc. 

    """
    
    def __init__(self, epochobject):
        self.epoch = epochobject
        gladefile = "epoch-property.glade"
        windowname = "notebookEpochProperty"
        self.wTree = gtk.glade.XML(gladefile, windowname)   
        self.prop = self.wTree.get_widget('notebookEpochProperty')

        self.wTree.signal_autoconnect(self)
        self.setupSessions()
        #signaldic = {"on_toggleRecord_toggled",
        self.epoch.sessionSignalCallback = self.updateSessions
        

        self.datatab = datatab.EpochDataTab(self.wTree.get_widget("treeviewDataSources"), 
                                       self.epoch.GetDataStates,
                                       self.epoch.SetDataState)

        self.buttonAddNote = self.wTree.get_widget("buttonAddNote")
        self.vboxNotes = self.wTree.get_widget('vboxExperimentNotes')

        self.notesgui = notesgui.NotesGUI(self.vboxNotes,
                                          self.buttonAddNote,
                                          self.epoch.GetNotes,
                                          self.epoch.CreateNote)
                                          

    def populate(self):
        """
        Fill out the widgets with the relevant values
        """
        
    def setupSessions(self):
        self.sessionstore = gtk.ListStore(int, int, str, int, str)
        
                
        treeview = self.wTree.get_widget("treeviewSessions")
        treeview.get_selection().set_mode(gtk.SELECTION_NONE)
        self.treeviewSessions = treeview
        treeview.set_model(self.sessionstore)

        # Session col
        sessioncol = gtk.TreeViewColumn("Session")

        cell = gtk.CellRendererText()
        sessioncol.pack_start(cell, True)
        sessioncol.add_attribute(cell, "text", 0)

        treeview.append_column(sessioncol)

        # Start TS col
        starttscol = gtk.TreeViewColumn("Start Timestamp")

        cell = gtk.CellRendererText()
        starttscol.pack_start(cell, True)
        starttscol.add_attribute(cell, "text", 1)

        treeview.append_column(starttscol)

        # Start  col
        starttimecol = gtk.TreeViewColumn("Start Time")

        cell = gtk.CellRendererText()
        starttimecol.pack_start(cell, True)
        starttimecol.add_attribute(cell, "text", 2)

        treeview.append_column(starttimecol)

        # Stop TS col
        stoptscol = gtk.TreeViewColumn("Stop Timestamp")

        cell = gtk.CellRendererText()
        stoptscol.pack_start(cell, True)
        stoptscol.add_attribute(cell, "text", 3)

        treeview.append_column(stoptscol)

        # Stop  col
        stoptimecol = gtk.TreeViewColumn("Stop Time")

        cell = gtk.CellRendererText()
        stoptimecol.pack_start(cell, True)
        stoptimecol.add_attribute(cell, "text", 4)

        treeview.append_column(stoptimecol)

        
        
        self.updateSessions()

    def updateSessions(self):
        self.sessionstore.clear()
        pos = 0
        for s in self.epoch.GetSessions():
            l = list(s)
            l.insert(0, pos)
            pos += 1
            self.sessionstore.append(l)
                
        
    def on_toggleRecord_toggled(self, widget):
        if widget.get_active():
            self.epoch.StartRecording()
        else:
            self.epoch.StopRecording()

class ExperimentProperty(object):
    """
    The property interface for an experiment
    """

    def __init__(self, expobj):
        self.experiment = expobj

        gladefile = "experiment-property.glade"
        windowname = "notebookExperimentProperty"
        self.wTree = gtk.glade.XML(gladefile, windowname)
        self.wTree.signal_autoconnect(self)
        
        self.prop = self.wTree.get_widget('notebookExperimentProperty')

        self.datatab = datatab.ExperimentDataTab(self.wTree.get_widget("treeviewDataSources"), 
                                       self.experiment.GetDataStates,
                                       self.experiment.SetDataState)
        
        #self.updateNotes()
        self.experimentLabel = self.wTree.get_widget("labelExperimentFilename")
        self.experimentLabel.props.label = "<big><big><big>%s</big></big></big>" % self.experiment.name
        

        self.buttonAddNote = self.wTree.get_widget("buttonAddNote")
        self.vboxNotes = self.wTree.get_widget('vboxExperimentNotes')

        self.notesgui = notesgui.NotesGUI(self.vboxNotes,
                                          self.buttonAddNote,
                                          self.experiment.GetNotes,
                                          self.experiment.CreateNote)
                                          
        

class RecorderApp(object):
    def __init__(self):
        gladefile = "main.glade"
        self.windowname = "main"
        
        self.wTree = gtk.glade.XML(gladefile, self.windowname)   

        
        self.wTree.signal_autoconnect(self)

        # load popup
        self.popupEpochTree = gtk.glade.XML(gladefile, "menuPopupEpochs")
        self.popupEpoch = self.popupEpochTree.get_widget("menuPopupEpochs")
        self.popupEpochTree.signal_autoconnect(self)
        self.recorder = Recorder()
        self.setupEpochs()

        # now connect the new-experiment and new-epoch widgets
        self.recorder.connect('experiment-create', self.experimentCreate)
        

        self.propertyPanes = {}
    def on_menuQuit_activate(self, widget):
        print "QUIT"
        gtk.main_quit()
        
    def on_treeviewExperiments_button_press_event(self, treeview, event):
        if event.button == 3:
            x = int(event.x)
            y = int(event.y)
            time = event.time
            pthinfo = treeview.get_path_at_pos(x, y)
            if pthinfo is not None:
                path, col, cellx, celly = pthinfo
                treeview.grab_focus()
                treeview.set_cursor( path, col, 0)
                self.popupEpoch.popup( None, None, None, event.button, time)
            return 1
    
    def experimentCreate(self, recorder, exp):
        ep = ExperimentProperty(exp)
        exp.connect('epoch-create', self.epochCreate)
        self.propertyPanes[exp] = ep
        
    def epochCreate(self, recorder, epoch):
        ep = EpochProperty(epoch)
        self.propertyPanes[epoch] = ep
        
    def setPropertyPane(self):
        box = self.wTree.get_widget("boxProperties")
        try:
            box.remove(box.get_children()[0])
        except:
            pass
        # now put in the correct one,
        selection = self.treeviewExperiments.get_selection()
        (model, iter) = selection.get_selected()
        obj = model.get_value(iter, 0)
        print obj
        pane = self.propertyPanes[obj]
        
        box.add(pane.prop)
        box.show()

    def on_treeviewExperiments_cursor_changed(self, widget):
        print "Cursor changed", widget
        
        self.setPropertyPane()
        
    def setupEpochs(self):
        # pass

        self.treestore = RecorderTreeModel(self.recorder)
                
        treeview = self.wTree.get_widget("treeviewExperiments")
        self.treeviewExperiments = treeview
        treeview.set_model(self.treestore)
        
        maincol = gtk.TreeViewColumn("Experiments and Epochs")

        cell = gtk.CellRendererText()
        maincol.pack_start(cell, True)
        maincol.add_attribute(cell, "text", 1)

        treeview.append_column(maincol)
        
    def on_menuNewExperiment_activate(self, widget):
        """
        Creating new experiment -- put up the new experiment
        dialog and everything
        
        """
        gladefile = "main.glade"
        windowname = "dialogCreateEpoch"
        try:
            epoch = gtk.glade.XML(gladefile, windowname)
            print "Running" 
            dialog =  epoch.get_widget("dialogCreateEpoch")
            retval = dialog.run()
            if retval == 1:
                # OK was hit, get the relvant values
                entry = epoch.get_widget("entryExperimentFilename")
                name=  entry.props.text
                self.recorder.CreateExperiment(name)
                # now set the cursor to the newest experiment
                
            else:
                pass
        except NameError:
            print "Name already exists" 
            pass
        dialog.hide()

    def on_menuitemAddEpoch_activate(self, widget):
        print "Adding Epoch"
        # get the current experiment
        selection = self.treeviewExperiments.get_selection()
        (model, iter) = selection.get_selected()
        expt = model.get_value(iter, 0)
        epoch = expt.CreateEpoch("Hello")
        
        
        
    def updateExperiment(self, pos):
        """
        We have our internal list of experiments, and
        here is where we find them 
        
        """
    
if __name__ == "__main__":
    app = RecorderApp()
    gtk.main()
