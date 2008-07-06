
import pygtk
pygtk.require('2.0')
import gtk
from  experiment import Experiment
from epoch import Epoch
from recorder import Recorder
import gobject

class RecorderTreeModel(gtk.GenericTreeModel):
    """
    http://www.pygtk.org/pygtk2tutorial/sec-GenericTreeModel.html

    what are we going to use as our iters?
    
    """
    
    column_types = (object, str, bool)
    column_names = ['reference', 'Name', 'isEpoch']

    def __init__(self, recorder):
        gtk.GenericTreeModel.__init__(self)
        self.recorder = recorder
        self.experiments = recorder.ListOpenExperiments()
        self.recorder.connect('experiment-create', self.recorderExperimentCreate)
        
    def recorderExperimentCreate(self, recorder, experiment):
        self.experiments = recorder.ListOpenExperiments()
        path = (len(self.experiments)-1,)
        iter = self.create_tree_iter(experiment)

        # now start capturing that experiment's signals
        experiment.connect('epoch-create', self.experimentEpochCreate)
        
        return self.row_inserted(path, iter)

    def experimentEpochCreate(self, expr, val):
        path = self.on_get_path(expr)
        iter = self.create_tree_iter(expr)
        self.row_has_child_toggled(path, iter)
        
    def experimentUpdated(self, expr, val):
        path = self.on_get_path(expr)
        iter = self.create_tree_iter(expr)
        self.row_has_child_toggled(path, iter)

    def iterToObject(self, iter):
        """
        Takes in a gtktreeiter and returns the underlying contained object
        
        """
    def objectToIter(self, object):
        """
        """
        iter = self.create_tree_iter(object)
        return iter
        
    def objectToPath(self, object):
        """
        """
        iter = self.objectToIter(object)
        return self.get_path(iter)
        
        
    def on_get_flags(self):
        return 0
            
    def on_get_n_columns(self):
        return len(self.column_types)
        
    def on_get_column_type(self, index):
        return self.column_types[index]
        
    def on_get_iter(self, path):
        if len(self.experiments) == 0:
            return None
        
        if len(path) > 1:
            return self.experiments[path[0]].epochsOrdered[path[1]]
        else:
            return self.experiments[path[0]]
            
    def on_get_path(self, rowref):
        exppos = 0
        if rowref == None:
            return (0)
        for i in self.experiments:
            if rowref == i:
                return (exppos,)
            epochpos = 0
            for j in i.epochsOrdered:
                if rowref == j:
                    return (exppos, epochpos)
                epochpos += 1
            exppos += 1

        
    def on_get_value(self, rowref, column):
        if column == 0:
            return rowref
        if column == 1:
            return rowref.name
        if column == 2:
            if isinstance(rowref, Epoch):
                return True
            else:
                return False
            
                
            
    def on_iter_next(self, rowref):
        if rowref == None:
            return self.experiments[0]
    
        path = self.on_get_path(rowref)
        try:
            if len(path) == 1:
                return self.experiments[path[0] + 1]
            else:
                return self.experiments[path[0]].epochsOrdered[path[1] + 1]
        except IndexError:
            return None

    
    def on_iter_children(self, parent):
        
        try:
            return parent.epochsOrdered[0]
        except:
            return None
            
    def on_iter_has_child(self, rowref):
        try:
            if rowref.epochs:
                return True
        except:
            return False
        
    def on_iter_n_children(self, rowref):
        try:
            return len(parent.epochs)
        except:
            return 0
        
    def on_iter_nth_child(self, parent, n):
        
        if parent == None:
            return self.experiments[n]
        else:
            try:
                return parent.epochsOrdered[n]
            except:
                return None
    
    def on_iter_parent(self, child):
        
        path =  self.on_get_path(child)
        if len(path) == 1:
            return None
        else:
            return self.experiments[path[0]]

def addexperiment(self, recorder):

    recorder.CreateExperiment("test experiment")
            
if __name__ == "__main__":
    """
    Simple test of the underlying model

    """

    w = gtk.Window()
##     treestore = gtk.TreeStore(str)
##     treestore.append(None, ("test",))

    recorder = Recorder()
    treestore = RecorderTreeModel(recorder)
    treeview = gtk.TreeView()

    maincol = gtk.TreeViewColumn("Experiments and Epochs")
    
    cell = gtk.CellRendererText()
    maincol.pack_start(cell, True)
    maincol.add_attribute(cell, "text", 1)
    
    treeview.append_column(maincol)
    vbox = gtk.VBox()
    button = gtk.Button("Add experiment")
    button.connect('clicked', addexperiment, recorder)
    w.add(vbox)
    vbox.pack_start(treeview)
    vbox.pack_start(button)
    w.show_all()
    treeview.set_model(treestore)

    testdict = {}
    
    
    for i in range(1):
        e = recorder.CreateExperiment("DoctorWho %d" % i)
        #e.connect('create-epoch', treestore.experimentUpdated)
        
        e.CreateEpoch("Hello %d" %i )
        e.CreateEpoch("Hello %d" % (i+1)  )

    gtk.main()

    
