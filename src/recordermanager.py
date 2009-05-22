"""
The Experiment Manager is the meta-process that manages experiment
recording instances. 

"""

import gobject

import dbus
import dbus.service
import dbus.mainloop.glib
import os
import os.path
from optparse import OptionParser

class ExperimentFactory(object):
    """
    Generic interface to allow wrapping
    """
    
    def __init__(self):
        pass


    def OpenExperiment(self, name, recordermanager):
        pass

    def CreateExpeirment(self, name, recordermanager):
        pass

class ExperimentFactoryMockup(ExperimentFactory):
    """
    A mock object for experiment creation

    """
    def __init__(self):
        self.dummy = 0
        
        pass

    def OpenExperiment(self, name, recordermanager):
        print "ExperimentFactoryMockup opening experiment", name
        recordermanager.RegisterExperiment(name, "dummyconnection%d" % self.dummy)
        self.dummy += 1

    def CreateExperiment(self, name, recordermanager):
        print "ExperimentFactoryMockup creating experiment", name
        recordermanager.RegisterExperiment(name, "dummyconnection%d" % self.dummy)
        self.dummy += 1

        pass
    

class RecorderManager(dbus.service.Object):
    def __init__(self, bus, loc, experiment_dir, experimentFactory):
        dbus.service.Object.__init__(self, bus, loc)
        self.bus = bus

        self.expFactory = experimentFactory
        # attempt to change tothe directory
        cd = os.getcwd()
        try:
            os.chdir(experiment_dir)
        except:
            raise Exception("Unable to open target experiment directory")
        os.chdir(cd)
        
        self.experiment_dir = experiment_dir

        self.open_experiments = {}
        

    @dbus.service.method("soma.Recording.Manager",
                         out_signature='as')
    def ListOpenExperiments(self):
        
        return self.open_experiments
    
    @dbus.service.method("soma.Recording.Manager",
                         out_signature='as')
    def ListAvailableExperiments(self):
        """
        List the availble .h5 files
        """
        
        expfiles = []
        for root, dirs, files in os.walk(self.experiment_dir):
            for f in files:
                extsplit = os.path.splitext(f)
                if len(extsplit) > 1:
                    print "file", f, extsplit
                    if extsplit[1] == '.h5':
                        expfiles.append(f)
                    
        return expfiles
    
    @dbus.service.method("soma.Recording.Manager",
                         in_signature='s')
    def OpenExperiment(self, expname):
        if not os.path.isfile(expname):
            raise Exception("Experiment not found")
        
        self.expFactory.OpenExperiment(expname, self)

    @dbus.service.method("soma.Recording.Manager",
                         in_signature='s')
    def CreateExperiment(self, expname):
        if os.path.isfile(expname):
            raise Exception("Experiment already exists")
        self.expFactory.CreateExperiment(expname, self)
            
    @dbus.service.method("soma.Recording.Manager",
                         out_signature="a{ss}")
    def GetStatistics(self):
        return {}

    @dbus.service.method("soma.Recording.ExperimentRegistry",
                         in_signature="ss")
    def RegisterExperiment(self, filename, dbusconn):
        self.open_experiments[filename] = dbusconn

    @dbus.service.method("soma.Recording.ExperimentRegistry",
                         in_signature="s")
    def UnregisterExperiment(self, dbusconn):
        pass

    
if __name__ == "__main__":

    
    parser = OptionParser()
    parser.add_option("-d", "--dbus", dest="dbus", help="dbus bus to connect to (defaults to Session bus)")
    

    (options, args) = parser.parse_args()
    
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    if options.dbus:
        session_bus = dbus.bus.BusConnection(options.dbus)
    else:
        session_bus = dbus.SessionBus()
        
    name = dbus.service.BusName("soma.recording.Manager", session_bus)
    ef = ExperimentFactoryMockup()
    
    object = RecorderManager(session_bus, '/manager', "expdir", ef)
    
    mainloop = gobject.MainLoop()
    mainloop.run()
