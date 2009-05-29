import tables


#h5file = tables.openFile("H5ExperimentRecording_stats.h5")
#print h5file.root.Rose.TSpike.Sink0
h5file = tables.openFile("H5ExperimentRecording_sessiontest.h5")
print h5file.root.Rose._v_attrs.sessions
