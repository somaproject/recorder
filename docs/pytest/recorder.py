import gobject

from experiment import Experiment

class Recorder(gobject.GObject):
    __gsignals__ = { 'experimentavailable': (gobject.SIGNAL_RUN_FIRST,
                                           gobject.TYPE_NONE,
                                           (gobject.TYPE_OBJECT,)) }
    
    def __init__(self):
        gobject.GObject.__init__(self)
        self.experiments = {}
        
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
        
    def CreateExperiment(self, name):
        """
        create a new experiment and return
        the object name
        """
        if name in self.experiments:
            raise NameError, "name %s already names an open experiment" % name
        
        e = Experiment(self, str(name))
        self.experiments[name] = e
        self.emit('experimentavailable', e)
        return e

