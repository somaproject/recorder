#!/usr/bin/env python

# example basictreeview.py

import pygtk
pygtk.require('2.0')
import gtk
import gobject
import dbus
import time

import re

class DataReceiverStatus(object):
    def processResultString(self, s):
        sl = s.split(", ")
        dhash = {}
        for v in sl:
            x = v.split("=")        
            dhash[x[0]] = int(x[1])

        return dhash

    def __init__(self, src, mstr):
        xre = re.compile("src=(\d+) typ=(\d+)")
        [srcstr, typstr] = xre.match(src).groups()
        
        rhash = self.processResultString(mstr)
        
        self.type = int(typstr)
        self.src = int(srcstr)
        self.pktCount = rhash['pktCount']
        self.latestSeq =  rhash['latestSeq'] 
        self.dupeCount =  rhash['dupeCount'] 
        self.pendingCount =  rhash['pendingCount'] 
        self.missingPacketCount =  rhash['missingPacketCount'] 
        self.reTxRxCount =  rhash['reTxRxCount']

    
class DataReceiverDbus(object):
    def __init__(self):
        self.bus = dbus.SessionBus()
        self.obj =  self.bus.get_object('org.soma.Recorder',
                                           '/org/soma/Recorder')
        self.recorder = dbus.Interface(self.obj,
                                       dbus_interface='org.soma.Recorder')
        
    def getStatus(self):
        rst = self.recorder.NetworkStats()
        
        drset = []
        for k, v in rst.iteritems():
            drs = DataReceiverStatus(k, v)
            drset.append(drs)
        return drset


collist = [('src', int),
           ('typ', int),
           ('pktCount', int), 
           ('latestSeq', int),
           ('dupes', int),
           ('pending', int),
           ('missing', int),
           ('reTxRx', int),
           ('rx Rate', float)
           ]

liststore = gtk.ListStore(int, int, int, int, int, int, int, int, float)
drdbus = DataReceiverDbus()

def updateListStore(drsupdate, timeSinceLastUpdate):
    for d in drsupdate:
        src = d.src
        typ = d.type
        pos = -1
        found = False
        for l in liststore:
            pos += 1
            if l[0] == src and l[1] == typ:
                found = True
                break
        dtuple = (d.src, d.type, d.pktCount, d.latestSeq,
                              d.dupeCount, d.pendingCount,
                              d.missingPacketCount,
                              d.reTxRxCount, 0.0)
        if not found:
            liststore.append(dtuple)
        else:
            # update pkt rate
            oldcnt = liststore[(pos,)][2]
            liststore[(pos,)] = dtuple
            newcnt = liststore[(pos,)][2]
            rate = float(newcnt - oldcnt) / timeSinceLastUpdate
            liststore.set(liststore.get_iter((pos, )), 8, newcnt - oldcnt)
    
class BasicTreeViewExample:

    # close the window and quit
    def delete_event(self, widget, event, data=None):
        gtk.main_quit()
        return False

    def __init__(self):
        # Create a new window
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)

        self.window.set_title("Basic TreeView Example")

        self.window.set_size_request(200, 200)

        self.window.connect("delete_event", self.delete_event)

        # create a TreeStore with one string column to use as the model


        
        # we'll add some data now - 4 rows with 3 child rows each

        self.treeview = gtk.TreeView(liststore)


        self.tvcols = []
        for i, v in enumerate(collist):
            # create the TreeViewColumn to display the data
            tvcolumn = gtk.TreeViewColumn(v[0])

            # create a CellRendererText to render the data
            cell = gtk.CellRendererText()
            # add the cell to the tvcolumn and allow it to expand
            tvcolumn.pack_start(cell, True)
            # set the cell "text" attribute to column 0 - retrieve text
            # from that column in treestore
            tvcolumn.add_attribute(cell, 'text', i)

            # Allow sorting on the column
            tvcolumn.set_sort_column_id(0)

            # add tvcolumn to treeview
            self.treeview.append_column(tvcolumn)
            self.tvcols.append(tvcolumn)

        self.window.add(self.treeview)

        self.window.show_all()


told = None

def updateData():
    drset = drdbus.getStatus()
    global told
    #liststore.append((7, 9))
    if told == None:
        told = time.time()
    else:
        tdelta = time.time() - told

        updateListStore(drset, tdelta)
        print "Update data", tdelta, told
    told = time.time()

    return True    

def main():
    source_id = gobject.timeout_add(1000, updateData)
    
    gtk.main()

if __name__ == "__main__":
    tvexample = BasicTreeViewExample()
    main()
