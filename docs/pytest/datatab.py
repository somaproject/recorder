
import sys
import pygtk
pygtk.require("2.0")
import gtk
import gobject

class DataTab(object):

    def __init__(self, treeviewwidget, getdatasets, setdataset):
        """
        treeviewwidget: the widget that we want to display
        getdatasets: function pointer
        setdataset: function pointer

        """
        self.treeview = treeviewwidget
        self.getdatasetsfunc = getdatasets
        self.setdatasetfunc = setdataset

        
        
        self.liststore = gtk.ListStore(*self.__coltypes__)
        self.treeview.set_model(self.liststore)

        
        # create the columns
        for i in range(len(self.__cols__)):
            col = gtk.TreeViewColumn(self.__cols__[i])

            if self.__coltypes__[i] == gobject.TYPE_BOOLEAN:
                cell = gtk.CellRendererToggle()
                cell.set_property('activatable', True)
                if i == 1:
                    cell.connect('toggled', self.enable_toggled, i)
                else:
                    cell.connect('toggled', self.source_toggled, i)
                col.pack_start(cell, True)
                col.add_attribute(cell, "active", i)
                
            else:
                cell = gtk.CellRendererText()
                cell.props.editable = True
                if i == self.__namecol__:
                    cell.connect('edited', self.source_edited)
                    col.props.expand = True
                else:
                    col.props.expand = False
                col.props.resizable = True
                col.pack_start(cell, True)
                col.add_attribute(cell, "text", i)
                col.add_attribute(cell, "text", i)
            
            self.treeview.append_column(col)

        self.treeview.append_column(gtk.TreeViewColumn())

        self.updateModel()

    def setEditable(self, isEditable):
        # sets the editable state
        
        for col in self.treeview.get_columns():
            # pass
            for cell in col.get_cell_renderers():
                cell.props.sensitive = isEditable
            
            
    def source_edited(self, cell, path, value):
        print "edited", cell, path, value
        pos = 2
        if self.__cols__[1] == "src":
            pos = 1
            
            
        iter = self.liststore.get_iter(path)
        val = self.liststore.get_value(iter, pos)
        self.liststore.set_value(iter, pos, value)
        
        self.rowUpdated(path)
        
    def source_toggled( self, cell, path, col):
        iter = self.liststore.get_iter(path)
        val = self.liststore.get_value(iter, col)

        self.liststore.set_value(iter, col, not val)
        self.rowUpdated(path)
        
class ExperimentDataTab(DataTab):
    __cols__ = ['src', 'enabled', 'name', 'tspike', 'wave', 'raw']
    __coltypes__ = [gobject.TYPE_INT,
                    gobject.TYPE_BOOLEAN, 
                    gobject.TYPE_STRING,
                    gobject.TYPE_BOOLEAN,
                    gobject.TYPE_BOOLEAN,
                    gobject.TYPE_BOOLEAN,
                    gobject.TYPE_BOOLEAN]
    __namecol__ = 2

    def __init__(self, treeviewwidget, getdatasets, setdataset):
        self.treeview = treeviewwidget
        self.getdatasetsfunc = getdatasets
        self.setdatasetfunc = setdataset


        self.treeview.get_selection().set_mode(gtk.SELECTION_MULTIPLE)
        self.liststore = gtk.ListStore(*self.__coltypes__)
        self.treeview.set_model(self.liststore)

        cells = []
        cols = []
        # create the columns
        for i in range(len(self.__cols__)):
            col = gtk.TreeViewColumn(self.__cols__[i])

            if self.__coltypes__[i] == gobject.TYPE_BOOLEAN:
                cell = gtk.CellRendererToggle()

                if i == 1:
                    cell.connect('toggled', self.enable_toggled, i)
                else:
                    cell.connect('toggled', self.source_toggled, i)
                cells.append(cell)
                col.pack_start(cell, True)
                col.add_attribute(cell, "active", i)
                if i == 1:
                    cell.set_property("activatable", True)
                else:
                    col.add_attribute(cell, 'activatable', 1)
            else:
                cell = gtk.CellRendererText()

                cells.append(cell)
                if i == self.__namecol__:
                    cell.connect('edited', self.source_edited)
                    col.props.expand = True
                col.pack_start(cell, True)
                col.add_attribute(cell, "text", i)
                col.add_attribute(cell, "text", i)
                if i != 0:
                    col.add_attribute(cell, 'editable', 1)                
            cols.append(col)
            self.treeview.append_column(col)

        # now make the others conditioned on the first:
        for cpos in range(2, len(cells)):
            cols[cpos].add_attribute(cells[cpos], 'sensitive', 1)

        self.updateModel()
        


    def updateModel(self):
        self.liststore.clear()
        self.data = self.getdatasetsfunc()
        for rowdata in self.data:
            # expand out the row a bit
            row = [rowdata[0], rowdata[1], rowdata[3]]
            for i in xrange(4):
                if i in rowdata[2]:
                    row.append(True)
                else:
                    row.append(False)
            
            self.liststore.append(row)

    def rowUpdated(self, pos):
        print "rowUpdated", pos
        iter = self.liststore.get_iter(pos)
        src = self.liststore.get_value(iter, 0)
        enabled = self.liststore.get_value(iter, 1)
        name = self.liststore.get_value(iter, 2)
        data = []
        for i in xrange(4):
            data.append(self.liststore.get_value(iter, 3 + i))
                
        self.setdatasetfunc(src, enabled, name, data)

    def enable_toggled( self, cell, path, col):
        print "Enable toggled"
        
        iter = self.liststore.get_iter(path)
        val = self.liststore.get_value(iter, col)

        self.liststore.set_value(iter, col, not val)
        self.rowUpdated(path)
            
class EpochDataTab(DataTab):
    __cols__ = ['src', 'name', 'tspike', 'wave', 'raw']
    __coltypes__ = [gobject.TYPE_INT,
                    gobject.TYPE_STRING,
                    gobject.TYPE_BOOLEAN,
                    gobject.TYPE_BOOLEAN,
                    gobject.TYPE_BOOLEAN,
                    gobject.TYPE_BOOLEAN]
    __namecol__ = 1

    def __init__(self, treeviewwidget, getdatasets, setdataset):
        DataTab.__init__(self,  treeviewwidget, getdatasets, setdataset)

        
    def updateModel(self):
        self.liststore.clear()
        self.data = self.getdatasetsfunc()
        for rowdata in self.data:
            # expand out the row a bit
            row = [rowdata[0], rowdata[2]]
            for i in xrange(4):
                if i in rowdata[1]:
                    row.append(True)
                else:
                    row.append(False)
            
            self.liststore.append(row)



