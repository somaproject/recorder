
/*
 *	This file was automatically generated by dbusxx-xml2cpp; DO NOT EDIT!
 */

#ifndef __dbusxx__dbus_glue_h__PROXY_MARSHAL_H
#define __dbusxx__dbus_glue_h__PROXY_MARSHAL_H

#include <dbus-c++/dbus.h>

namespace org {
namespace freedesktop {

class DBus
 : public ::DBus::InterfaceProxy
{
public:

    DBus()
    : ::DBus::InterfaceProxy("org.freedesktop.DBus")
    {
        connect_signal(DBus, NameOwnerChanged, _NameOwnerChanged_stub);
        connect_signal(DBus, NameLost, _NameLost_stub);
        connect_signal(DBus, NameAcquired, _NameAcquired_stub);
    }

public:

    /* methods exported by this interface,
     * this functions will invoke the corresponding methods on the remote objects
     */
    ::DBus::UInt32 RequestName( const ::DBus::String& argin0, const ::DBus::UInt32& argin1 )
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << argin0;
        wi << argin1;
        call.member("RequestName");
        ::DBus::Message ret = invoke_method(call);
        ::DBus::MessageIter ri = ret.reader();

        ::DBus::UInt32 argout;
        ri >> argout;
        return argout;
    }

    ::DBus::UInt32 ReleaseName( const ::DBus::String& argin0 )
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << argin0;
        call.member("ReleaseName");
        ::DBus::Message ret = invoke_method(call);
        ::DBus::MessageIter ri = ret.reader();

        ::DBus::UInt32 argout;
        ri >> argout;
        return argout;
    }

    ::DBus::UInt32 StartServiceByName( const ::DBus::String& argin0, const ::DBus::UInt32& argin1 )
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << argin0;
        wi << argin1;
        call.member("StartServiceByName");
        ::DBus::Message ret = invoke_method(call);
        ::DBus::MessageIter ri = ret.reader();

        ::DBus::UInt32 argout;
        ri >> argout;
        return argout;
    }

    ::DBus::String Hello(  )
    {
        ::DBus::CallMessage call;
        call.member("Hello");
        ::DBus::Message ret = invoke_method(call);
        ::DBus::MessageIter ri = ret.reader();

        ::DBus::String argout;
        ri >> argout;
        return argout;
    }

    ::DBus::Bool NameHasOwner( const ::DBus::String& argin0 )
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << argin0;
        call.member("NameHasOwner");
        ::DBus::Message ret = invoke_method(call);
        ::DBus::MessageIter ri = ret.reader();

        ::DBus::Bool argout;
        ri >> argout;
        return argout;
    }

    std::vector< ::DBus::String > ListNames(  )
    {
        ::DBus::CallMessage call;
        call.member("ListNames");
        ::DBus::Message ret = invoke_method(call);
        ::DBus::MessageIter ri = ret.reader();

        std::vector< ::DBus::String > argout;
        ri >> argout;
        return argout;
    }

    void AddMatch( const ::DBus::String& argin0 )
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << argin0;
        call.member("AddMatch");
        ::DBus::Message ret = invoke_method(call);
    }

    void RemoveMatch( const ::DBus::String& argin0 )
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << argin0;
        call.member("RemoveMatch");
        ::DBus::Message ret = invoke_method(call);
    }

    ::DBus::String GetNameOwner( const ::DBus::String& argin0 )
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << argin0;
        call.member("GetNameOwner");
        ::DBus::Message ret = invoke_method(call);
        ::DBus::MessageIter ri = ret.reader();

        ::DBus::String argout;
        ri >> argout;
        return argout;
    }

    std::vector< ::DBus::String > ListQueuedOwners( const ::DBus::String& argin0 )
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << argin0;
        call.member("ListQueuedOwners");
        ::DBus::Message ret = invoke_method(call);
        ::DBus::MessageIter ri = ret.reader();

        std::vector< ::DBus::String > argout;
        ri >> argout;
        return argout;
    }

    ::DBus::UInt32 GetConnectionUnixUser( const ::DBus::String& argin0 )
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << argin0;
        call.member("GetConnectionUnixUser");
        ::DBus::Message ret = invoke_method(call);
        ::DBus::MessageIter ri = ret.reader();

        ::DBus::UInt32 argout;
        ri >> argout;
        return argout;
    }

    ::DBus::UInt32 GetConnectionUnixProcessID( const ::DBus::String& argin0 )
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << argin0;
        call.member("GetConnectionUnixProcessID");
        ::DBus::Message ret = invoke_method(call);
        ::DBus::MessageIter ri = ret.reader();

        ::DBus::UInt32 argout;
        ri >> argout;
        return argout;
    }

    std::vector< ::DBus::Byte > GetConnectionSELinuxSecurityContext( const ::DBus::String& argin0 )
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << argin0;
        call.member("GetConnectionSELinuxSecurityContext");
        ::DBus::Message ret = invoke_method(call);
        ::DBus::MessageIter ri = ret.reader();

        std::vector< ::DBus::Byte > argout;
        ri >> argout;
        return argout;
    }

    void ReloadConfig(  )
    {
        ::DBus::CallMessage call;
        call.member("ReloadConfig");
        ::DBus::Message ret = invoke_method(call);
    }


public:

    /* signal handlers for this interface
     */
    virtual void NameOwnerChanged( const ::DBus::String& argin0, const ::DBus::String& argin1, const ::DBus::String& argin2 ) = 0;
    virtual void NameLost( const ::DBus::String& argin0 ) = 0;
    virtual void NameAcquired( const ::DBus::String& argin0 ) = 0;

private:

    /* unmarshalers (to unpack the DBus message before calling the actual signal handler)
     */
    void _NameOwnerChanged_stub( const ::DBus::SignalMessage& sig )
    {
        ::DBus::MessageIter ri = sig.reader();

        ::DBus::String arg0; ri >> arg0;
        ::DBus::String arg1; ri >> arg1;
        ::DBus::String arg2; ri >> arg2;
        NameOwnerChanged(arg0, arg1, arg2);
    }
    void _NameLost_stub( const ::DBus::SignalMessage& sig )
    {
        ::DBus::MessageIter ri = sig.reader();

        ::DBus::String arg0; ri >> arg0;
        NameLost(arg0);
    }
    void _NameAcquired_stub( const ::DBus::SignalMessage& sig )
    {
        ::DBus::MessageIter ri = sig.reader();

        ::DBus::String arg0; ri >> arg0;
        NameAcquired(arg0);
    }
};

} } 
#endif//__dbusxx__dbus_glue_h__PROXY_MARSHAL_H
