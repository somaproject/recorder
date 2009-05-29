import dbus
bus = dbus.SessionBus()
object = bus.get_object('org.soma.Recorder','/org/soma/Recorder')
recorder = dbus.Interface(object, dbus_interface='org.soma.Recorder')
#print echo.Random()
print recorder.createFile("testing")
#print echo.Sum([123, 234, 95, 520])
#print echo.Info()

