// **********************************************************************
//
// Copyright (c) 2003-2007 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

// Ice version 3.2.1
// Generated from file `pingreply.ice'

#ifndef __pingreply_h__
#define __pingreply_h__

#include <Ice/LocalObjectF.h>
#include <Ice/ProxyF.h>
#include <Ice/ObjectF.h>
#include <Ice/Exception.h>
#include <Ice/LocalObject.h>
#include <Ice/Proxy.h>
#include <Ice/Object.h>
#include <Ice/Outgoing.h>
#include <Ice/Incoming.h>
#include <Ice/Direct.h>
#include <Ice/StreamF.h>
#include <Ice/UndefSysMacros.h>

#ifndef ICE_IGNORE_VERSION
#   if ICE_INT_VERSION / 100 != 302
#       error Ice version mismatch!
#   endif
#   if ICE_INT_VERSION % 100 > 50
#       error Beta header file detected
#   endif
#   if ICE_INT_VERSION % 100 < 1
#       error Ice patch level mismatch!
#   endif
#endif

namespace IceProxy
{

namespace orca
{

namespace util
{

class LatencyPinger;
bool operator==(const LatencyPinger&, const LatencyPinger&);
bool operator!=(const LatencyPinger&, const LatencyPinger&);
bool operator<(const LatencyPinger&, const LatencyPinger&);
bool operator<=(const LatencyPinger&, const LatencyPinger&);
bool operator>(const LatencyPinger&, const LatencyPinger&);
bool operator>=(const LatencyPinger&, const LatencyPinger&);

class LatencyReplier;
bool operator==(const LatencyReplier&, const LatencyReplier&);
bool operator!=(const LatencyReplier&, const LatencyReplier&);
bool operator<(const LatencyReplier&, const LatencyReplier&);
bool operator<=(const LatencyReplier&, const LatencyReplier&);
bool operator>(const LatencyReplier&, const LatencyReplier&);
bool operator>=(const LatencyReplier&, const LatencyReplier&);

}

}

}

namespace orca
{

namespace util
{

class LatencyPinger;
bool operator==(const LatencyPinger&, const LatencyPinger&);
bool operator!=(const LatencyPinger&, const LatencyPinger&);
bool operator<(const LatencyPinger&, const LatencyPinger&);
bool operator<=(const LatencyPinger&, const LatencyPinger&);
bool operator>(const LatencyPinger&, const LatencyPinger&);
bool operator>=(const LatencyPinger&, const LatencyPinger&);

class LatencyReplier;
bool operator==(const LatencyReplier&, const LatencyReplier&);
bool operator!=(const LatencyReplier&, const LatencyReplier&);
bool operator<(const LatencyReplier&, const LatencyReplier&);
bool operator<=(const LatencyReplier&, const LatencyReplier&);
bool operator>(const LatencyReplier&, const LatencyReplier&);
bool operator>=(const LatencyReplier&, const LatencyReplier&);

}

}

namespace IceInternal
{

void incRef(::orca::util::LatencyPinger*);
void decRef(::orca::util::LatencyPinger*);

void incRef(::IceProxy::orca::util::LatencyPinger*);
void decRef(::IceProxy::orca::util::LatencyPinger*);

void incRef(::orca::util::LatencyReplier*);
void decRef(::orca::util::LatencyReplier*);

void incRef(::IceProxy::orca::util::LatencyReplier*);
void decRef(::IceProxy::orca::util::LatencyReplier*);

}

namespace orca
{

namespace util
{

typedef ::IceInternal::Handle< ::orca::util::LatencyPinger> LatencyPingerPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::orca::util::LatencyPinger> LatencyPingerPrx;

void __write(::IceInternal::BasicStream*, const LatencyPingerPrx&);
void __read(::IceInternal::BasicStream*, LatencyPingerPrx&);
void __write(::IceInternal::BasicStream*, const LatencyPingerPtr&);
void __patch__LatencyPingerPtr(void*, ::Ice::ObjectPtr&);

void __addObject(const LatencyPingerPtr&, ::IceInternal::GCCountMap&);
bool __usesClasses(const LatencyPingerPtr&);
void __decRefUnsafe(const LatencyPingerPtr&);
void __clearHandleUnsafe(LatencyPingerPtr&);

typedef ::IceInternal::Handle< ::orca::util::LatencyReplier> LatencyReplierPtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::orca::util::LatencyReplier> LatencyReplierPrx;

void __write(::IceInternal::BasicStream*, const LatencyReplierPrx&);
void __read(::IceInternal::BasicStream*, LatencyReplierPrx&);
void __write(::IceInternal::BasicStream*, const LatencyReplierPtr&);
void __patch__LatencyReplierPtr(void*, ::Ice::ObjectPtr&);

void __addObject(const LatencyReplierPtr&, ::IceInternal::GCCountMap&);
bool __usesClasses(const LatencyReplierPtr&);
void __decRefUnsafe(const LatencyReplierPtr&);
void __clearHandleUnsafe(LatencyReplierPtr&);

}

}

namespace orca
{

namespace util
{

typedef ::std::vector< ::Ice::Double> DoubleSequence;

class __U__DoubleSequence { };

struct PingReplyData
{
    ::Ice::Int id;
    ::orca::util::DoubleSequence doubles;

    bool operator==(const PingReplyData&) const;
    bool operator!=(const PingReplyData&) const;
    bool operator<(const PingReplyData&) const;
    bool operator<=(const PingReplyData& __rhs) const
    {
        return operator<(__rhs) || operator==(__rhs);
    }
    bool operator>(const PingReplyData& __rhs) const
    {
        return !operator<(__rhs) && !operator==(__rhs);
    }
    bool operator>=(const PingReplyData& __rhs) const
    {
        return !operator<(__rhs);
    }

    void __write(::IceInternal::BasicStream*) const;
    void __read(::IceInternal::BasicStream*);
};

}

}

namespace IceProxy
{

namespace orca
{

namespace util
{

class LatencyPinger : virtual public ::IceProxy::Ice::Object
{
public:

    void takeItBack(const ::orca::util::PingReplyData& data)
    {
        takeItBack(data, 0);
    }
    void takeItBack(const ::orca::util::PingReplyData& data, const ::Ice::Context& __ctx)
    {
        takeItBack(data, &__ctx);
    }
    
private:

    void takeItBack(const ::orca::util::PingReplyData&, const ::Ice::Context*);
    
public:
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
};

class LatencyReplier : virtual public ::IceProxy::Ice::Object
{
public:

    void setPinger(const ::orca::util::LatencyPingerPrx& server)
    {
        setPinger(server, 0);
    }
    void setPinger(const ::orca::util::LatencyPingerPrx& server, const ::Ice::Context& __ctx)
    {
        setPinger(server, &__ctx);
    }
    
private:

    void setPinger(const ::orca::util::LatencyPingerPrx&, const ::Ice::Context*);
    
public:

    void takeIt(const ::orca::util::PingReplyData& data)
    {
        takeIt(data, 0);
    }
    void takeIt(const ::orca::util::PingReplyData& data, const ::Ice::Context& __ctx)
    {
        takeIt(data, &__ctx);
    }
    
private:

    void takeIt(const ::orca::util::PingReplyData&, const ::Ice::Context*);
    
public:

    ::orca::util::PingReplyData takeItAndReturn(const ::orca::util::PingReplyData& data)
    {
        return takeItAndReturn(data, 0);
    }
    ::orca::util::PingReplyData takeItAndReturn(const ::orca::util::PingReplyData& data, const ::Ice::Context& __ctx)
    {
        return takeItAndReturn(data, &__ctx);
    }
    
private:

    ::orca::util::PingReplyData takeItAndReturn(const ::orca::util::PingReplyData&, const ::Ice::Context*);
    
public:

    void shutdown()
    {
        shutdown(0);
    }
    void shutdown(const ::Ice::Context& __ctx)
    {
        shutdown(&__ctx);
    }
    
private:

    void shutdown(const ::Ice::Context*);
    
public:
    
    static const ::std::string& ice_staticId();

private: 

    virtual ::IceInternal::Handle< ::IceDelegateM::Ice::Object> __createDelegateM();
    virtual ::IceInternal::Handle< ::IceDelegateD::Ice::Object> __createDelegateD();
};

}

}

}

namespace IceDelegate
{

namespace orca
{

namespace util
{

class LatencyPinger : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual void takeItBack(const ::orca::util::PingReplyData&, const ::Ice::Context*) = 0;
};

class LatencyReplier : virtual public ::IceDelegate::Ice::Object
{
public:

    virtual void setPinger(const ::orca::util::LatencyPingerPrx&, const ::Ice::Context*) = 0;

    virtual void takeIt(const ::orca::util::PingReplyData&, const ::Ice::Context*) = 0;

    virtual ::orca::util::PingReplyData takeItAndReturn(const ::orca::util::PingReplyData&, const ::Ice::Context*) = 0;

    virtual void shutdown(const ::Ice::Context*) = 0;
};

}

}

}

namespace IceDelegateM
{

namespace orca
{

namespace util
{

class LatencyPinger : virtual public ::IceDelegate::orca::util::LatencyPinger,
                      virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual void takeItBack(const ::orca::util::PingReplyData&, const ::Ice::Context*);
};

class LatencyReplier : virtual public ::IceDelegate::orca::util::LatencyReplier,
                       virtual public ::IceDelegateM::Ice::Object
{
public:

    virtual void setPinger(const ::orca::util::LatencyPingerPrx&, const ::Ice::Context*);

    virtual void takeIt(const ::orca::util::PingReplyData&, const ::Ice::Context*);

    virtual ::orca::util::PingReplyData takeItAndReturn(const ::orca::util::PingReplyData&, const ::Ice::Context*);

    virtual void shutdown(const ::Ice::Context*);
};

}

}

}

namespace IceDelegateD
{

namespace orca
{

namespace util
{

class LatencyPinger : virtual public ::IceDelegate::orca::util::LatencyPinger,
                      virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual void takeItBack(const ::orca::util::PingReplyData&, const ::Ice::Context*);
};

class LatencyReplier : virtual public ::IceDelegate::orca::util::LatencyReplier,
                       virtual public ::IceDelegateD::Ice::Object
{
public:

    virtual void setPinger(const ::orca::util::LatencyPingerPrx&, const ::Ice::Context*);

    virtual void takeIt(const ::orca::util::PingReplyData&, const ::Ice::Context*);

    virtual ::orca::util::PingReplyData takeItAndReturn(const ::orca::util::PingReplyData&, const ::Ice::Context*);

    virtual void shutdown(const ::Ice::Context*);
};

}

}

}

namespace orca
{

namespace util
{

class LatencyPinger : virtual public ::Ice::Object
{
public:

    typedef LatencyPingerPrx ProxyType;
    typedef LatencyPingerPtr PointerType;
    
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual void takeItBack(const ::orca::util::PingReplyData&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___takeItBack(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::IceInternal::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

void __patch__LatencyPingerPtr(void*, ::Ice::ObjectPtr&);

class LatencyReplier : virtual public ::Ice::Object
{
public:

    typedef LatencyReplierPrx ProxyType;
    typedef LatencyReplierPtr PointerType;
    
    virtual ::Ice::ObjectPtr ice_clone() const;

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::Current()) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::Current()) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::Current()) const;
    static const ::std::string& ice_staticId();

    virtual void setPinger(const ::orca::util::LatencyPingerPrx&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___setPinger(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void takeIt(const ::orca::util::PingReplyData&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___takeIt(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::orca::util::PingReplyData takeItAndReturn(const ::orca::util::PingReplyData&, const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___takeItAndReturn(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void shutdown(const ::Ice::Current& = ::Ice::Current()) = 0;
    ::IceInternal::DispatchStatus ___shutdown(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual ::IceInternal::DispatchStatus __dispatch(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual void __write(::IceInternal::BasicStream*) const;
    virtual void __read(::IceInternal::BasicStream*, bool);
    virtual void __write(const ::Ice::OutputStreamPtr&) const;
    virtual void __read(const ::Ice::InputStreamPtr&, bool);
};

void __patch__LatencyReplierPtr(void*, ::Ice::ObjectPtr&);

}

}

#endif
