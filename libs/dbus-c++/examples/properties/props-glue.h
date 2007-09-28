
/*
 *	This file was automatically generated by dbusxx-xml2cpp; DO NOT EDIT!
 */

#ifndef __dbusxx__props_glue_h__ADAPTOR_MARSHAL_H
#define __dbusxx__props_glue_h__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>

namespace org {
namespace freedesktop {
namespace DBus {

class PropsDemo
: public ::DBus::InterfaceAdaptor
{
public:

    PropsDemo()
    : ::DBus::InterfaceAdaptor("org.freedesktop.DBus.PropsDemo")
    {
        bind_property(Version, "i", true, false);
        bind_property(Message, "s", true, true);
    }

    ::DBus::IntrospectedInterface* const introspect() const 
    {
        static ::DBus::IntrospectedArgument MessageChanged_args[] = 
        {
            { "message", "s", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedMethod PropsDemo_methods[] = 
        {
            { 0, 0 }
        };
        static ::DBus::IntrospectedMethod PropsDemo_signals[] = 
        {
            { "MessageChanged", MessageChanged_args },
            { 0, 0 }
        };
        static ::DBus::IntrospectedProperty PropsDemo_properties[] = 
        {
            { "Version", "i", true, false },
            { "Message", "s", true, true },
            { 0, 0, 0, 0 }
        };
        static ::DBus::IntrospectedInterface PropsDemo_interface = 
        {
            "org.freedesktop.DBus.PropsDemo",
            PropsDemo_methods,
            PropsDemo_signals,
            PropsDemo_properties
        };
        return &PropsDemo_interface;
    }

public:

    /* properties exposed by this interface, use
     * property() and property(value) to get and set a particular property
     */
    ::DBus::PropertyAdaptor< ::DBus::Int32 > Version;
    ::DBus::PropertyAdaptor< ::DBus::String > Message;

public:

    /* methods exported by this interface,
     * you will have to implement them in your ObjectAdaptor
     */

public:

    /* signal emitters for this interface
     */
    void MessageChanged( const ::DBus::String& arg1 )
    {
        ::DBus::SignalMessage sig("MessageChanged");
        ::DBus::MessageIter wi = sig.writer();
        wi << arg1;
        emit_signal(sig);
    }

private:

    /* unmarshalers (to unpack the DBus message before calling the actual interface method)
     */
};

} } } 
#endif//__dbusxx__props_glue_h__ADAPTOR_MARSHAL_H
