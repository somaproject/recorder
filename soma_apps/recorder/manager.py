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
        try:
            p = subprocess.Popen([expbin])
            p.terminate()
            
        except OSError as (errno, errstr):
            if errno == 2:
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
    
class Manager(dbus.service.Object):
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

        # fire the signal 
        self.ExperimentAvailable(str(dbusconn))
        

    @dbus.service.method("soma.recording.ExperimentRegistry",
                         sender_keyword='dbusconn')
    def Unregister(self, dbusconn=None):
        for i, v in self.open_experiments.iteritems():
            if v == dbusconn:
                del self.open_experiments[i]
                break


    @dbus.service.signal(dbus_interface="soma.recording.Manager",
                         signature="s")
    def ExperimentAvailable(self, objconn):
        pass
    
