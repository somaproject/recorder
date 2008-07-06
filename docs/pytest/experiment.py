from epoch import Epoch
import gobject


class Experiment(gobject.GObject):
    __gsignals__ = { 'epoch-create': (gobject.SIGNAL_RUN_FIRST,
                                      gobject.TYPE_NONE,
                                      (gobject.TYPE_OBJECT,)) }
    
    def __init__(self, recorder, name):
        self.recorder = recorder
        gobject.GObject.__init__(self)
        self.epochsOrdered = []
        self.epochs = {}
        self.name = name

        self.datasources = []
        for i in xrange(64):
           self.datasources.append((i, True, [], "SOURCE%2.2d" % i))
        
        self.notes = []

        self.properties = {"user" : "jonas",
                           "create" : "2008-01-01 22:33"}
        
    def GetFileProperties(self):
        return self.properties
    
    def GetEpochs(self):
        return self.epochsOrdered

    def CreateEpoch(self, name):
        if name in self.epochs:
            raise "Epoch name already exists"
        
        e = Epoch(name)
        self.epochsOrdered.append(e)
        self.epochs[name] = e
        self.emit("epoch-create", e)
        return e

    def deleteEpoch(self, name):
        if name not in self.epochs:
            raise "Epoch name not found"
        e = self.epochs[name]
        self.epochs.erase(e)
        self.epochsOrdered.remove(e)
        
        
    def epochListUpdated():
        pass
    
    def CreateNote(self, title, text):
        """

        Add a textual note; return the objref for the note, returns the
        id

        """
        print "create note", title, text

        self.notes.append((0, 0, title, text))
        
        
    def GetNotes(self):
        """
        Get all of the notes, which are tuples of (ts, time,  title, text)
        
        """
        return self.notes
    
    def DeleteNote(self, id):
        """

        """

    def SetDataState(self, src, available, enabled, name):
        """
        src is the src #
        enabled is a list of the types that are enabled
        name is the name
        
        """
        new = (src, available, enabled, name)
        self.datasources[src] = new
        print "Experiment::SetDataState", new
        
    def GetDataStates(self):
        """
        Returns ALL OF THE DATA Configuration

         [ (0, available, [0, 2, 3], Name), 
         (1, available, [1, 2], Name) ]
        """
        return self.datasources
    
