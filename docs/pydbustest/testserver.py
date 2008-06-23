import dbus

session_bus = dbus.SessionBus()

# bus name == name of application you want to talk to
# object path -- applications export many objects

class Example(dbus.service.Object):
    def __init__(self, object_path):
        dbus.service.Object.__init__(self, dbus.SessionBus(), path)

    @dbus.service.method(dbus_interface='com.example.Sample',
                         in_signature='v', out_signature='s')
    def StringifyVariant(self, variant):
        return str(variant)

