import gobject
import time

class Session(object):

    def __init__(self, startts, starttime):
        self.startts = startts
        self.starttime = starttime
        self.stopts =  startts
        self.stoptime = starttime
        
    def toTuple(self):
        return (self.startts, self.starttime,
                self.stopts, self.stoptime)
    
class Epoch(gobject.GObject):
    """
    The individual epoch 

    """
    
    __gsignals__ = { 'renamed': (gobject.SIGNAL_RUN_FIRST,
                                     gobject.TYPE_NONE,
                                     (gobject.TYPE_STRING,))}

    def __init__(self, parent, name):
        gobject.GObject.__init__(self)
        self.name = name
        self.parent = parent
        self.timer_id = gobject.timeout_add(500, self.updateClock)
        self.ts = 0
        self.sessions = []
        self.recording = False

        self.sessionSignalCallback = None

        self.datasinks = set()
        self.datasinknames = {}
        
        for i in xrange(64):
           self.datasinknames[i] = "Source%0.2d" % i 
        
        self.notes = []

    def GetName(self):
        return self.name
    
    def setName(self, name):
        self.name = name
        self.emit("renamed", name)
        
        
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
    

    def GetStats(self):
        """
        The epochs stats... not entirely clear what these are yet! 

        """

    def EnableDataSink(self, src, typ):
        """
        src is the src #
        type is the type
        """

        self.datasinks.append(src, typ)
        
        
    def DisableDataSink(self, src, typ):
        """
        """
        self.datasinks.remove(src, typ)
        

    def GetDataSinks(self):
        """
        Returns a list of string tuples
        """
        return list(self.datasinks)

    def SetDataName(self, src, name):
        """

        """
        self.datasinknames[src] = name


    def GetDataName(self, src):
        """

        """
        return self.datasinknames[src]
    
        
    
    def AddEventRXMask(self, enableEvent, cmdlist):
        """
        Recive the cartesian product of sources and commands
        
        """

    def GetEventRXMask(self, source, cmdlist):
        """
        returns a list of all the events that we receive

        """
        
    def RemoveEventRXMask(self, source, cmdlist):
        """
        Remove the cartesian product of the event mask
        """
        
    def StartRecording(self):
        """
        begin recording for this epoch
        Can steal recording from another epoch
        
        """
        print "Epoch", self.name, "starting recording"
        self.sessions.append(Session(self.ts, time.time()))
        self.recording = True
        self.parent.setRecording(True)

    def StopRecording(self):
        """
        Stop the recording
        """
        print "Epoch", self.name, "stopping recording"
        self.recording = False
        if self.sessionSignalCallback != None:
            self.sessionSignalCallback()

        self.parent.setRecording(False)
        
    def updateClock(self):
        self.ts += 25000
        if self.recording:
            self.sessions[-1].stoptime = time.time()
            self.sessions[-1].stopts= self.ts

        if self.sessionSignalCallback != None:
            self.sessionSignalCallback()

            
        return True

    def PauseRecording(self):
        """
        Pause the recording

        """

    def GetSessions(self):
        """
        Get epoch recording sessions
        """

        # convert the sessions
        
        return [s.toTuple() for s in self.sessions]
    
    
