import gobject
import dbus
import dbus.service
import dbus.mainloop.glib

class Recorder(dbus.service.Object):
    """
    Primary dbus object for recorder
    """

    def ListAvailableExperiments(self):
        """
        Returns a list of possible experiments, by name
        """

    def ListOpenExperiments(self):
        """
        Returns the list of all availabe open experiments,
        by object path
        """
        
    def OpenExperiment(self, name):
        """
        Open the named experiment and return the object name
        """

    def CreateExperiment(self, name):
        """
        create a new experiment and return
        the object name
        """
    def CloseExperiment(self, name):
        """
        Close the experiment

        """


class Experiment(dbus.service.Object):
    """
    Experiment object


    has some properties like name, record date, create date, etc.

    
    """

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

    def AddNote(self, notestr):
        """
        Add a textual note

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

    def GetNotes(self):
        """
        return a textual note
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
        
