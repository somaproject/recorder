The Soma Experiment Timeline
==================================

Recording the temporal occurrence of events is the essence of any
scientific data acquisition system. Soma keeps track of all time
internally using a 50 kHz, 64-bit timestamp that starts at zero upon
device boot and won't wrap-around for another 11 million years. 

We create the notion of an experimental "session", which we will call
a Soma *experiment*. This is not meant to map to the scientist's
notion of his experiment, but rather a "session" within his
experiment. In the Wilson lab where we generally run the animal
on a particular task each day, an *experiment* would correspond
to a single day's worth of recording. At a given point in time, a
particular experiment is "active". 

We break an experiment down into *epochs*. An epoch is meant to denote
a section of an experiment with contiguous state -- both at the
hardware and software level. A typical set of epochs within an
experiment in the Wilson lab might be "pre", "sleep0", "run0",
"sleep1", "run1", "sleep2", and "post". It is important
to note that when an experiment is active, there is always at least
one epoch active. 

Epoch transitions generally happen at the request of the experimenter,
although the transition can easily be scripted. In general, however,
an experimenter will tell Soma "switch to epoch 'run'" via either GUI
or command-line.

While it is the intention that epochs will be traversed consecutively
in time, this is not a requirement. That is, if an experimenter wishes
to go back to an epoch, or have an "idle" epoch that they repeatedly
transition to, the system will not prevent this.

Other pieces of the Soma software system can query the current epoch, and
be notified when the "active" epoch changes. 

Managing experiment / epoch notification
-----------------------------------------

Recording Manager: 
.CreateExperiment()
.OpenExperiment()
.CloseExperiment()
.ListOpenExperiments()

signal: experimentAvailable

experiment: 
TimelineMonitor interface: 
   .GetAllEpochs()	
   .EpochChange() [ signal] 
   .GetReferenceTime()
   .ReferenceTimeChange()
