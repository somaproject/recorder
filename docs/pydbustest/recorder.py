import gobject
import dbus
import dbus.service
import dbus.mainloop.glib

class Recorder(dbus.service.Object):
    """
    Primary dbus object for Recorder api

    
    """
    def __init__(self, bus, object_path):
        dbus.service.Object.__init__(self, bus, object_path)
        self._last_input = None
        self.experiments = {}
        self.bus = bus
    
    def ListAvailableExperiments(self):
        """
        Returns a list of possible experiments, by name. This
        is effectively the available files in the "experiments" directory
        """

    @dbus.service.method("soma.Recorder", out_signature = "ao")
    def ListOpenExperiments(self):
        """
        Returns the list of all availabe open experiments,
        by object path
        """
        return [v for k, v in self.experiments.iteritems()]
    
        
    def OpenExperiment(self, name):
        """
        Open the named experiment and return the object name
        """

    @dbus.service.method("soma.Recorder", in_signature='s',
                         out_signature = 'o')
    def CreateExperiment(self, name):
        """
        create a new experiment and return
        the object name
        """
        e = Experiment(self, self.bus, str(name))
        self.experiments[name] = e

        return e

    
##     @dbus.service.method("org.freedesktop.DBus.Properties")
    def closeExperiment(self, name):
        """
        A non-exported method telling the recorder to remove the
        experiment from its list. 
        """

        self.experiments.pop(name)
        

class Experiment(dbus.service.Object):
    """
    Experiment object

    Has properties like name, record date, create date, etc.
    

    """
    def __init__(self, recorder, bus, name):
        self.bus = bus
        self.name = name
        dbus.service.Object.__init__(self, bus, "/Experiments/%s" % self.name)
        self.recorder = recorder
        
    @dbus.service.method("soma.Recorder.Experiment")
    def Close(self):
        """
        Close the experiment

        """
        self.recorder.closeExperiment(self.name)
        self.remove_from_connection()
        
        
    def GetEpochs(self):
        """
        Returns a list of all epochs by object path?

        """

    def CreateEpoch(self, name):
        """
        creates an epoch, returns the epoch path

        """

    def DeleteEpoch(self, epochpath):
        """
        deletes an epoch
        """

    def GetEpoch(self, epochname):
        """
        return an epoch object path by name

        """

class Epoch(dbus.service.Object):
    """
    The individual epoch 

    """

    def CreateNote(self, notepath):
        """
        Add a textual note; return the objref for the note

        """
        
    def GetNotes(self, notestr):
        """
        Get all of the notes, via object path
        """
        
    def DeleteNote(self, notepath):
        """

        """

    def GetStats(self):
        """
        The epochs stats

        """

    def DataEnable(self, src, type):
        """
        """

    def DataDisable(self, src, type):
        """
        """

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

    def StopRecording(self):
        """
        Stop the recording
        """

    def PauseRecording(self):
        """
        Pause the recording

        """

    def GetSessions(self):
        """
        Get epoch recording sessions
        """
        
