import pygtk

#!/usr/bin/env python

# example helloworld.py

import pygtk
pygtk.require('2.0')
import gtk
import localrecord

class HelloWorld:

    cols = ["pendingCount", "outOfOrderCount", "dupeCount",
            "pktCount", "missingPacketCount", "reTxRxCount", "latestSeq"]

    # This is a callback function. The data arguments are ignored
    # in this example. More on callbacks below.
    def hello(self, widget, data=None):
        print "Hello World"

    def delete_event(self, widget, event, data=None):
        # If you return FALSE in the "delete_event" signal handler,
        # GTK will emit the "destroy" signal. Returning TRUE means
        # you don't want the window to be destroyed.
        # This is useful for popping up 'are you sure you want to quit?'
        # type dialogs.
        print "delete event occurred"

        # Change FALSE to TRUE and the main window will not be destroyed
        # with a "delete_event".
        return False

    def destroy(self, widget, data=None):
        print "destroy signal occurred"
        gtk.main_quit()

    def __init__(self):
        # create a new window
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
    
        # When the window is given the "delete_event" signal (this is given
        # by the window manager, usually by the "close" option, or on the
        # titlebar), we ask it to call the delete_event () function
        # as defined above. The data passed to the callback
        # function is NULL and is ignored in the callback function.
        self.window.connect("delete_event", self.delete_event)
    
        # Here we connect the "destroy" event to a signal handler.  
        # This event occurs when we call gtk_widget_destroy() on the window,
        # or if we return FALSE in the "delete_event" callback.
        self.window.connect("destroy", self.destroy)
    
        # Sets the border width of the window.
        self.window.set_border_width(10)
    

        self.liststore = gtk.ListStore(str, str, str,  str, str, str, str, str)

        self.treeview = gtk.TreeView(self.liststore)
        allcols = ["src"]
        allcols += self.cols
        for i, v in enumerate(allcols):
            
            tvcolumn = gtk.TreeViewColumn(v)
            self.treeview.append_column(tvcolumn)
            cell = gtk.CellRendererText()
            tvcolumn.pack_start(cell, True)
            tvcolumn.add_attribute(cell, 'text', i)
        
        # This packs the button into the window (a GTK container).
        self.window.add(self.treeview)
    
        # The final step is to display this newly created widget.
        self.treeview.show()
        
        # and the window
        self.window.show()

    def main(self):
        # All PyGTK applications must have a gtk.main(). Control ends here
        # and waits for an event to occur (like a key press or mouse event).
        gtk.main()

        secdelay = 5
        query_timeout = gobject.timeout_add(secdelay * 1000, self.updateStats)
        self.currentEpoch = None
        
    def updateStats(self):
        # get sources :
        if currentEpoch:
            dp = self.currentEpoch.GetDataSinks()
        self.liststore.clear()

        for datasinkstr in dp:
            (src, typ) = eval(datasinkstr)

            
        
# If the program is run directly or passed as an argument to the python
# interpreter then create a HelloWorld instance and show it

def setupExpt():

    proxy = bus.get_object(runner.name,
                           "/soma/recording/recorder")
    iface = dbus.Interface(proxy, "soma.recording.Recorder")
    iface.ListOpenExperiments()
    
    fname = getTempFileName()
    iface.CreateExperiment(fname)
    
    while (len(iface.ListOpenExperiments()) == 0):
        time.sleep(1)

    expconn = iface.ListOpenExperiments()[0]
    print "The dbus connection is", expconn
    expobj = bus.get_object(expconn, "/soma/recording/experiment")
    exp = dbus.Interface(expobj, "soma.recording.Experiment")

    exp.GetFileProperties()
    # CREATE EPOCHS
    epochRose = exp.CreateEpoch("Rose")
    epochRose = bus.get_object(expconn, epochRose)
    epochRose = dbus.Interface(epochRose, "soma.recording.Epoch")
    
    # ENABLE DATA IN EPOCHS
    N = 10 
    for i in range(N):
        epochRose.EnableDataSink(i, 0)

    epochRose.StartRecording()
    time.sleep(60)
    
    epochRose.StopRecording()
    for i in range(N):
        stats = epochRose.GetSinkSessionStatistics(i, 0, 0)
        for k, v in stats.iteritems():
            print "  ", k, "=", v

    runner.close()
    
if __name__ == "__main__":

    
    hello = HelloWorld()

    
    hello.main()
