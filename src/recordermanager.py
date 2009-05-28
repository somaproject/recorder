#!/usr/bin/python
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
import subprocess
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
        recordermanager.Register(name, "dummyconnection%d" % self.dummy)
        self.dummy += 1

    def CreateExperiment(self, name, recordermanager):
        fid = file(name, 'w')
        fid.write("Hello world")
        fid.close()
        
        recordermanager.Register(name, "dummyconnection%d" % self.dummy)
        self.dummy += 1

        pass
    
class ExperimentServerFactory(ExperimentFactory):
    """
    The actual object for creating experiment servers
    """
    def __init__(self, expbin):
        self.expbin = expbin
        if not os.path.exists(expbin):
            raise NotImplementedError("Requested experiment binary, %s, cannot be found" % expbin)


    def OpenExperiment(self, name, recordermanager):
        pass

    def CreateExperiment(self, name, recordermanager):
        args = [self.expbin,
                "--create-file=%s" % name,
                "--dbus=%s" % recordermanager.bus.address,
                "--soma-ip=%s" % recordermanager.somaIP]
        proc = subprocess.Popen(args)
        return proc
    
class RecorderManager(dbus.service.Object):
    def __init__(self, bus, loc, somaip, experiment_dir, experimentFactory):
        dbus.service.Object.__init__(self, bus, loc)
        self.bus = bus
        self.somaIP = somaip
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
        

    @dbus.service.method("soma.recording.Manager",
                         out_signature='as')
    def ListOpenExperiments(self):
        
        return self.open_experiments.values()
    
    @dbus.service.method("soma.recording.Manager",
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
                    if extsplit[1] == '.h5':
                        expfiles.append(f)
                    
        return expfiles
    
    @dbus.service.method("soma.recording.Manager",
                         in_signature='s')
    def OpenExperiment(self, expname):
        if not os.path.isfile(expname):
            raise Exception("Experiment not found")
        
        self.expFactory.OpenExperiment(expname, self)

    @dbus.service.method("soma.recording.Manager",
                         in_signature='s')
    def CreateExperiment(self, expname):
        
        if os.path.isfile(expname):
            raise Exception("Experiment already exists")
        self.expFactory.CreateExperiment(os.path.join(self.experiment_dir, expname),
                                         self)
            
    @dbus.service.method("soma.recording.Manager",
                         out_signature="a{ss}")
    def GetStatistics(self):
        return {}

    @dbus.service.method("soma.recording.ExperimentRegistry",
                         in_signature="s", sender_keyword='dbusconn')
    def Register(self, filename, dbusconn=None):
        self.open_experiments[filename] = dbusconn

    @dbus.service.method("soma.recording.ExperimentRegistry",
                         sender_keyword='dbusconn')
    def Unregister(self, dbusconn=None):
        for i, v in self.open_experiments.iteritems():
            if v == dbusconn:
                del self.open_experiments[i]
                break
    
if __name__ == "__main__":

    
    parser = OptionParser()
    parser.add_option("-i", "--soma-ip", dest="somaip", help="The IP address of the Soma hardware")
    parser.add_option("-d", "--dbus", dest="dbus", help="dbus bus to connect to (defaults to Session bus)")
    parser.add_option("-e", "--expdir", dest="expdir", help="directory for experiments")
    parser.add_option("-m", "--mock", action="store_true",
                      dest="mock", help="use mock experiment server (for testing)")
    parser.add_option("-b", "--expbin", dest="expbin",
                      help="The location of the experiment server binary")
    
    (options, args) = parser.parse_args()
    
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    if options.dbus:
        session_bus = dbus.bus.BusConnection(options.dbus)
        session_bus.address = options.dbus
    else:
        session_bus = dbus.SessionBus()

    if not options.expdir:
        raise Exception("Must specify experiment directory with --expdir")

        
    name = dbus.service.BusName("soma.recording.Manager", session_bus)
    if options.mock:
        ef = ExperimentFactoryMockup()
    else:
        ef = ExperimentServerFactory(options.expbin)
    
    
    object = RecorderManager(session_bus, '/manager', options.somaip, options.expdir, ef)
    
    mainloop = gobject.MainLoop()
    mainloop.run()
