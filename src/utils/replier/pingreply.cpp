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

#include <pingreply.h>
#include <Ice/LocalException.h>
#include <Ice/ObjectFactory.h>
#include <Ice/BasicStream.h>
#include <Ice/Object.h>
#include <IceUtil/Iterator.h>
#include <IceUtil/ScopedArray.h>

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

static const ::std::string __orca__util__LatencyPinger__takeItBack_name = "takeItBack";

static const ::std::string __orca__util__LatencyReplier__setPinger_name = "setPinger";

static const ::std::string __orca__util__LatencyReplier__takeIt_name = "takeIt";

static const ::std::string __orca__util__LatencyReplier__takeItAndReturn_name = "takeItAndReturn";

static const ::std::string __orca__util__LatencyReplier__shutdown_name = "shutdown";

void
IceInternal::incRef(::orca::util::LatencyPinger* p)
{
    p->__incRef();
}

void
IceInternal::decRef(::orca::util::LatencyPinger* p)
{
    p->__decRef();
}

void
IceInternal::incRef(::IceProxy::orca::util::LatencyPinger* p)
{
    p->__incRef();
}

void
IceInternal::decRef(::IceProxy::orca::util::LatencyPinger* p)
{
    p->__decRef();
}

void
IceInternal::incRef(::orca::util::LatencyReplier* p)
{
    p->__incRef();
}

void
IceInternal::decRef(::orca::util::LatencyReplier* p)
{
    p->__decRef();
}

void
IceInternal::incRef(::IceProxy::orca::util::LatencyReplier* p)
{
    p->__incRef();
}

void
IceInternal::decRef(::IceProxy::orca::util::LatencyReplier* p)
{
    p->__decRef();
}

void
orca::util::__write(::IceInternal::BasicStream* __os, const ::orca::util::LatencyPingerPrx& v)
{
    __os->write(::Ice::ObjectPrx(v));
}

void
orca::util::__read(::IceInternal::BasicStream* __is, ::orca::util::LatencyPingerPrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::orca::util::LatencyPinger;
        v->__copyFrom(proxy);
    }
}

void
orca::util::__write(::IceInternal::BasicStream* __os, const ::orca::util::LatencyPingerPtr& v)
{
    __os->write(::Ice::ObjectPtr(v));
}

void
orca::util::__write(::IceInternal::BasicStream* __os, const ::orca::util::LatencyReplierPrx& v)
{
    __os->write(::Ice::ObjectPrx(v));
}

void
orca::util::__read(::IceInternal::BasicStream* __is, ::orca::util::LatencyReplierPrx& v)
{
    ::Ice::ObjectPrx proxy;
    __is->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new ::IceProxy::orca::util::LatencyReplier;
        v->__copyFrom(proxy);
    }
}

void
orca::util::__write(::IceInternal::BasicStream* __os, const ::orca::util::LatencyReplierPtr& v)
{
    __os->write(::Ice::ObjectPtr(v));
}

bool
orca::util::PingReplyData::operator==(const PingReplyData& __rhs) const
{
    return !operator!=(__rhs);
}

bool
orca::util::PingReplyData::operator!=(const PingReplyData& __rhs) const
{
    if(this == &__rhs)
    {
        return false;
    }
    if(id != __rhs.id)
    {
        return true;
    }
    if(doubles != __rhs.doubles)
    {
        return true;
    }
    return false;
}

bool
orca::util::PingReplyData::operator<(const PingReplyData& __rhs) const
{
    if(this == &__rhs)
    {
        return false;
    }
    if(id < __rhs.id)
    {
        return true;
    }
    else if(__rhs.id < id)
    {
        return false;
    }
    if(doubles < __rhs.doubles)
    {
        return true;
    }
    else if(__rhs.doubles < doubles)
    {
        return false;
    }
    return false;
}

void
orca::util::PingReplyData::__write(::IceInternal::BasicStream* __os) const
{
    __os->write(id);
    if(doubles.size() == 0)
    {
        __os->writeSize(0);
    }
    else
    {
        __os->write(&doubles[0], &doubles[0] + doubles.size());
    }
}

void
orca::util::PingReplyData::__read(::IceInternal::BasicStream* __is)
{
    __is->read(id);
    __is->read(doubles);
}

void
orca::util::__addObject(const LatencyPingerPtr& p, ::IceInternal::GCCountMap& c)
{
    p->__addObject(c);
}

bool
orca::util::__usesClasses(const LatencyPingerPtr& p)
{
    return p->__usesClasses();
}

void
orca::util::__decRefUnsafe(const LatencyPingerPtr& p)
{
    p->__decRefUnsafe();
}

void
orca::util::__clearHandleUnsafe(LatencyPingerPtr& p)
{
    p.__clearHandleUnsafe();
}

void
orca::util::__addObject(const LatencyReplierPtr& p, ::IceInternal::GCCountMap& c)
{
    p->__addObject(c);
}

bool
orca::util::__usesClasses(const LatencyReplierPtr& p)
{
    return p->__usesClasses();
}

void
orca::util::__decRefUnsafe(const LatencyReplierPtr& p)
{
    p->__decRefUnsafe();
}

void
orca::util::__clearHandleUnsafe(LatencyReplierPtr& p)
{
    p.__clearHandleUnsafe();
}

void
IceProxy::orca::util::LatencyPinger::takeItBack(const ::orca::util::PingReplyData& data, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __delBase = __getDelegate();
            ::IceDelegate::orca::util::LatencyPinger* __del = dynamic_cast< ::IceDelegate::orca::util::LatencyPinger*>(__delBase.get());
            __del->takeItBack(data, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

const ::std::string&
IceProxy::orca::util::LatencyPinger::ice_staticId()
{
    return ::orca::util::LatencyPinger::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::orca::util::LatencyPinger::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::orca::util::LatencyPinger);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::orca::util::LatencyPinger::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::orca::util::LatencyPinger);
}

bool
IceProxy::orca::util::operator==(const ::IceProxy::orca::util::LatencyPinger& l, const ::IceProxy::orca::util::LatencyPinger& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) == static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::orca::util::operator!=(const ::IceProxy::orca::util::LatencyPinger& l, const ::IceProxy::orca::util::LatencyPinger& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) != static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::orca::util::operator<(const ::IceProxy::orca::util::LatencyPinger& l, const ::IceProxy::orca::util::LatencyPinger& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) < static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::orca::util::operator<=(const ::IceProxy::orca::util::LatencyPinger& l, const ::IceProxy::orca::util::LatencyPinger& r)
{
    return l < r || l == r;
}

bool
IceProxy::orca::util::operator>(const ::IceProxy::orca::util::LatencyPinger& l, const ::IceProxy::orca::util::LatencyPinger& r)
{
    return !(l < r) && !(l == r);
}

bool
IceProxy::orca::util::operator>=(const ::IceProxy::orca::util::LatencyPinger& l, const ::IceProxy::orca::util::LatencyPinger& r)
{
    return !(l < r);
}

void
IceProxy::orca::util::LatencyReplier::setPinger(const ::orca::util::LatencyPingerPrx& server, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __delBase = __getDelegate();
            ::IceDelegate::orca::util::LatencyReplier* __del = dynamic_cast< ::IceDelegate::orca::util::LatencyReplier*>(__delBase.get());
            __del->setPinger(server, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

void
IceProxy::orca::util::LatencyReplier::takeIt(const ::orca::util::PingReplyData& data, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __delBase = __getDelegate();
            ::IceDelegate::orca::util::LatencyReplier* __del = dynamic_cast< ::IceDelegate::orca::util::LatencyReplier*>(__delBase.get());
            __del->takeIt(data, __ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

::orca::util::PingReplyData
IceProxy::orca::util::LatencyReplier::takeItAndReturn(const ::orca::util::PingReplyData& data, const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __checkTwowayOnly(__orca__util__LatencyReplier__takeItAndReturn_name);
            __delBase = __getDelegate();
            ::IceDelegate::orca::util::LatencyReplier* __del = dynamic_cast< ::IceDelegate::orca::util::LatencyReplier*>(__delBase.get());
            return __del->takeItAndReturn(data, __ctx);
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapper(__delBase, __ex);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

void
IceProxy::orca::util::LatencyReplier::shutdown(const ::Ice::Context* __ctx)
{
    int __cnt = 0;
    while(true)
    {
        ::IceInternal::Handle< ::IceDelegate::Ice::Object> __delBase;
        try
        {
            __delBase = __getDelegate();
            ::IceDelegate::orca::util::LatencyReplier* __del = dynamic_cast< ::IceDelegate::orca::util::LatencyReplier*>(__delBase.get());
            __del->shutdown(__ctx);
            return;
        }
        catch(const ::IceInternal::LocalExceptionWrapper& __ex)
        {
            __handleExceptionWrapperRelaxed(__delBase, __ex, __cnt);
        }
        catch(const ::Ice::LocalException& __ex)
        {
            __handleException(__delBase, __ex, __cnt);
        }
    }
}

const ::std::string&
IceProxy::orca::util::LatencyReplier::ice_staticId()
{
    return ::orca::util::LatencyReplier::ice_staticId();
}

::IceInternal::Handle< ::IceDelegateM::Ice::Object>
IceProxy::orca::util::LatencyReplier::__createDelegateM()
{
    return ::IceInternal::Handle< ::IceDelegateM::Ice::Object>(new ::IceDelegateM::orca::util::LatencyReplier);
}

::IceInternal::Handle< ::IceDelegateD::Ice::Object>
IceProxy::orca::util::LatencyReplier::__createDelegateD()
{
    return ::IceInternal::Handle< ::IceDelegateD::Ice::Object>(new ::IceDelegateD::orca::util::LatencyReplier);
}

bool
IceProxy::orca::util::operator==(const ::IceProxy::orca::util::LatencyReplier& l, const ::IceProxy::orca::util::LatencyReplier& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) == static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::orca::util::operator!=(const ::IceProxy::orca::util::LatencyReplier& l, const ::IceProxy::orca::util::LatencyReplier& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) != static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::orca::util::operator<(const ::IceProxy::orca::util::LatencyReplier& l, const ::IceProxy::orca::util::LatencyReplier& r)
{
    return static_cast<const ::IceProxy::Ice::Object&>(l) < static_cast<const ::IceProxy::Ice::Object&>(r);
}

bool
IceProxy::orca::util::operator<=(const ::IceProxy::orca::util::LatencyReplier& l, const ::IceProxy::orca::util::LatencyReplier& r)
{
    return l < r || l == r;
}

bool
IceProxy::orca::util::operator>(const ::IceProxy::orca::util::LatencyReplier& l, const ::IceProxy::orca::util::LatencyReplier& r)
{
    return !(l < r) && !(l == r);
}

bool
IceProxy::orca::util::operator>=(const ::IceProxy::orca::util::LatencyReplier& l, const ::IceProxy::orca::util::LatencyReplier& r)
{
    return !(l < r);
}

void
IceDelegateM::orca::util::LatencyPinger::takeItBack(const ::orca::util::PingReplyData& data, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __orca__util__LatencyPinger__takeItBack_name, ::Ice::Normal, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        data.__write(__os);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateM::orca::util::LatencyReplier::setPinger(const ::orca::util::LatencyPingerPrx& server, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __orca__util__LatencyReplier__setPinger_name, ::Ice::Idempotent, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        ::orca::util::__write(__os, server);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateM::orca::util::LatencyReplier::takeIt(const ::orca::util::PingReplyData& data, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __orca__util__LatencyReplier__takeIt_name, ::Ice::Normal, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        data.__write(__os);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

::orca::util::PingReplyData
IceDelegateM::orca::util::LatencyReplier::takeItAndReturn(const ::orca::util::PingReplyData& data, const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __orca__util__LatencyReplier__takeItAndReturn_name, ::Ice::Normal, __context, __compress);
    try
    {
        ::IceInternal::BasicStream* __os = __og.os();
        data.__write(__os);
    }
    catch(const ::Ice::LocalException& __ex)
    {
        __og.abort(__ex);
    }
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
        ::orca::util::PingReplyData __ret;
        __ret.__read(__is);
        return __ret;
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateM::orca::util::LatencyReplier::shutdown(const ::Ice::Context* __context)
{
    ::IceInternal::Outgoing __og(__connection.get(), __reference.get(), __orca__util__LatencyReplier__shutdown_name, ::Ice::Idempotent, __context, __compress);
    bool __ok = __og.invoke();
    try
    {
        ::IceInternal::BasicStream* __is = __og.is();
        if(!__ok)
        {
            try
            {
                __is->throwException();
            }
            catch(const ::Ice::UserException& __ex)
            {
                throw ::Ice::UnknownUserException(__FILE__, __LINE__, __ex.ice_name());
            }
        }
    }
    catch(const ::Ice::LocalException& __ex)
    {
        throw ::IceInternal::LocalExceptionWrapper(__ex, false);
    }
}

void
IceDelegateD::orca::util::LatencyPinger::takeItBack(const ::orca::util::PingReplyData& data, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __orca__util__LatencyPinger__takeItBack_name, ::Ice::Normal, __context);
    while(true)
    {
        ::IceInternal::Direct __direct(__current);
        try
        {
            ::orca::util::LatencyPinger* __servant = dynamic_cast< ::orca::util::LatencyPinger*>(__direct.servant().get());
            if(!__servant)
            {
                ::Ice::OperationNotExistException __opEx(__FILE__, __LINE__);
                __opEx.id = __current.id;
                __opEx.facet = __current.facet;
                __opEx.operation = __current.operation;
                throw __opEx;
            }
            try
            {
                __servant->takeItBack(data, __current);
            }
            catch(const ::Ice::LocalException& __ex)
            {
                throw ::IceInternal::LocalExceptionWrapper(__ex, false);
            }
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
        return;
    }
}

void
IceDelegateD::orca::util::LatencyReplier::setPinger(const ::orca::util::LatencyPingerPrx& server, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __orca__util__LatencyReplier__setPinger_name, ::Ice::Idempotent, __context);
    while(true)
    {
        ::IceInternal::Direct __direct(__current);
        try
        {
            ::orca::util::LatencyReplier* __servant = dynamic_cast< ::orca::util::LatencyReplier*>(__direct.servant().get());
            if(!__servant)
            {
                ::Ice::OperationNotExistException __opEx(__FILE__, __LINE__);
                __opEx.id = __current.id;
                __opEx.facet = __current.facet;
                __opEx.operation = __current.operation;
                throw __opEx;
            }
            try
            {
                __servant->setPinger(server, __current);
            }
            catch(const ::Ice::LocalException& __ex)
            {
                throw ::IceInternal::LocalExceptionWrapper(__ex, false);
            }
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
        return;
    }
}

void
IceDelegateD::orca::util::LatencyReplier::takeIt(const ::orca::util::PingReplyData& data, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __orca__util__LatencyReplier__takeIt_name, ::Ice::Normal, __context);
    while(true)
    {
        ::IceInternal::Direct __direct(__current);
        try
        {
            ::orca::util::LatencyReplier* __servant = dynamic_cast< ::orca::util::LatencyReplier*>(__direct.servant().get());
            if(!__servant)
            {
                ::Ice::OperationNotExistException __opEx(__FILE__, __LINE__);
                __opEx.id = __current.id;
                __opEx.facet = __current.facet;
                __opEx.operation = __current.operation;
                throw __opEx;
            }
            try
            {
                __servant->takeIt(data, __current);
            }
            catch(const ::Ice::LocalException& __ex)
            {
                throw ::IceInternal::LocalExceptionWrapper(__ex, false);
            }
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
        return;
    }
}

::orca::util::PingReplyData
IceDelegateD::orca::util::LatencyReplier::takeItAndReturn(const ::orca::util::PingReplyData& data, const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __orca__util__LatencyReplier__takeItAndReturn_name, ::Ice::Normal, __context);
    while(true)
    {
        ::IceInternal::Direct __direct(__current);
        ::orca::util::PingReplyData __ret;
        try
        {
            ::orca::util::LatencyReplier* __servant = dynamic_cast< ::orca::util::LatencyReplier*>(__direct.servant().get());
            if(!__servant)
            {
                ::Ice::OperationNotExistException __opEx(__FILE__, __LINE__);
                __opEx.id = __current.id;
                __opEx.facet = __current.facet;
                __opEx.operation = __current.operation;
                throw __opEx;
            }
            try
            {
                __ret = __servant->takeItAndReturn(data, __current);
            }
            catch(const ::Ice::LocalException& __ex)
            {
                throw ::IceInternal::LocalExceptionWrapper(__ex, false);
            }
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
        return __ret;
    }
}

void
IceDelegateD::orca::util::LatencyReplier::shutdown(const ::Ice::Context* __context)
{
    ::Ice::Current __current;
    __initCurrent(__current, __orca__util__LatencyReplier__shutdown_name, ::Ice::Idempotent, __context);
    while(true)
    {
        ::IceInternal::Direct __direct(__current);
        try
        {
            ::orca::util::LatencyReplier* __servant = dynamic_cast< ::orca::util::LatencyReplier*>(__direct.servant().get());
            if(!__servant)
            {
                ::Ice::OperationNotExistException __opEx(__FILE__, __LINE__);
                __opEx.id = __current.id;
                __opEx.facet = __current.facet;
                __opEx.operation = __current.operation;
                throw __opEx;
            }
            try
            {
                __servant->shutdown(__current);
            }
            catch(const ::Ice::LocalException& __ex)
            {
                throw ::IceInternal::LocalExceptionWrapper(__ex, false);
            }
        }
        catch(...)
        {
            __direct.destroy();
            throw;
        }
        __direct.destroy();
        return;
    }
}

::Ice::ObjectPtr
orca::util::LatencyPinger::ice_clone() const
{
    throw ::Ice::CloneNotImplementedException(__FILE__, __LINE__);
    return 0; // to avoid a warning with some compilers
}

static const ::std::string __orca__util__LatencyPinger_ids[2] =
{
    "::Ice::Object",
    "::orca::util::LatencyPinger"
};

bool
orca::util::LatencyPinger::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__orca__util__LatencyPinger_ids, __orca__util__LatencyPinger_ids + 2, _s);
}

::std::vector< ::std::string>
orca::util::LatencyPinger::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__orca__util__LatencyPinger_ids[0], &__orca__util__LatencyPinger_ids[2]);
}

const ::std::string&
orca::util::LatencyPinger::ice_id(const ::Ice::Current&) const
{
    return __orca__util__LatencyPinger_ids[1];
}

const ::std::string&
orca::util::LatencyPinger::ice_staticId()
{
    return __orca__util__LatencyPinger_ids[1];
}

::IceInternal::DispatchStatus
orca::util::LatencyPinger::___takeItBack(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::orca::util::PingReplyData data;
    data.__read(__is);
    takeItBack(data, __current);
    return ::IceInternal::DispatchOK;
}

static ::std::string __orca__util__LatencyPinger_all[] =
{
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "takeItBack"
};

::IceInternal::DispatchStatus
orca::util::LatencyPinger::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__orca__util__LatencyPinger_all, __orca__util__LatencyPinger_all + 5, current.operation);
    if(r.first == r.second)
    {
        return ::IceInternal::DispatchOperationNotExist;
    }

    switch(r.first - __orca__util__LatencyPinger_all)
    {
        case 0:
        {
            return ___ice_id(in, current);
        }
        case 1:
        {
            return ___ice_ids(in, current);
        }
        case 2:
        {
            return ___ice_isA(in, current);
        }
        case 3:
        {
            return ___ice_ping(in, current);
        }
        case 4:
        {
            return ___takeItBack(in, current);
        }
    }

    assert(false);
    return ::IceInternal::DispatchOperationNotExist;
}

void
orca::util::LatencyPinger::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
orca::util::LatencyPinger::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
orca::util::LatencyPinger::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type orca::util::LatencyPinger was not generated with stream support";
    throw ex;
}

void
orca::util::LatencyPinger::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type orca::util::LatencyPinger was not generated with stream support";
    throw ex;
}

void 
orca::util::__patch__LatencyPingerPtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::orca::util::LatencyPingerPtr* p = static_cast< ::orca::util::LatencyPingerPtr*>(__addr);
    assert(p);
    *p = ::orca::util::LatencyPingerPtr::dynamicCast(v);
    if(v && !*p)
    {
        ::Ice::UnexpectedObjectException e(__FILE__, __LINE__);
        e.type = v->ice_id();
        e.expectedType = ::orca::util::LatencyPinger::ice_staticId();
        throw e;
    }
}

bool
orca::util::operator==(const ::orca::util::LatencyPinger& l, const ::orca::util::LatencyPinger& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
orca::util::operator!=(const ::orca::util::LatencyPinger& l, const ::orca::util::LatencyPinger& r)
{
    return static_cast<const ::Ice::Object&>(l) != static_cast<const ::Ice::Object&>(r);
}

bool
orca::util::operator<(const ::orca::util::LatencyPinger& l, const ::orca::util::LatencyPinger& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}

bool
orca::util::operator<=(const ::orca::util::LatencyPinger& l, const ::orca::util::LatencyPinger& r)
{
    return l < r || l == r;
}

bool
orca::util::operator>(const ::orca::util::LatencyPinger& l, const ::orca::util::LatencyPinger& r)
{
    return !(l < r) && !(l == r);
}

bool
orca::util::operator>=(const ::orca::util::LatencyPinger& l, const ::orca::util::LatencyPinger& r)
{
    return !(l < r);
}

::Ice::ObjectPtr
orca::util::LatencyReplier::ice_clone() const
{
    throw ::Ice::CloneNotImplementedException(__FILE__, __LINE__);
    return 0; // to avoid a warning with some compilers
}

static const ::std::string __orca__util__LatencyReplier_ids[2] =
{
    "::Ice::Object",
    "::orca::util::LatencyReplier"
};

bool
orca::util::LatencyReplier::ice_isA(const ::std::string& _s, const ::Ice::Current&) const
{
    return ::std::binary_search(__orca__util__LatencyReplier_ids, __orca__util__LatencyReplier_ids + 2, _s);
}

::std::vector< ::std::string>
orca::util::LatencyReplier::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&__orca__util__LatencyReplier_ids[0], &__orca__util__LatencyReplier_ids[2]);
}

const ::std::string&
orca::util::LatencyReplier::ice_id(const ::Ice::Current&) const
{
    return __orca__util__LatencyReplier_ids[1];
}

const ::std::string&
orca::util::LatencyReplier::ice_staticId()
{
    return __orca__util__LatencyReplier_ids[1];
}

::IceInternal::DispatchStatus
orca::util::LatencyReplier::___setPinger(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::orca::util::LatencyPingerPrx server;
    ::orca::util::__read(__is, server);
    setPinger(server, __current);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
orca::util::LatencyReplier::___takeIt(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::orca::util::PingReplyData data;
    data.__read(__is);
    takeIt(data, __current);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
orca::util::LatencyReplier::___takeItAndReturn(::IceInternal::Incoming&__inS, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Normal, __current.mode);
    ::IceInternal::BasicStream* __is = __inS.is();
    ::IceInternal::BasicStream* __os = __inS.os();
    ::orca::util::PingReplyData data;
    data.__read(__is);
    ::orca::util::PingReplyData __ret = takeItAndReturn(data, __current);
    __ret.__write(__os);
    return ::IceInternal::DispatchOK;
}

::IceInternal::DispatchStatus
orca::util::LatencyReplier::___shutdown(::IceInternal::Incoming&, const ::Ice::Current& __current)
{
    __checkMode(::Ice::Idempotent, __current.mode);
    shutdown(__current);
    return ::IceInternal::DispatchOK;
}

static ::std::string __orca__util__LatencyReplier_all[] =
{
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping",
    "setPinger",
    "shutdown",
    "takeIt",
    "takeItAndReturn"
};

::IceInternal::DispatchStatus
orca::util::LatencyReplier::__dispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair< ::std::string*, ::std::string*> r = ::std::equal_range(__orca__util__LatencyReplier_all, __orca__util__LatencyReplier_all + 8, current.operation);
    if(r.first == r.second)
    {
        return ::IceInternal::DispatchOperationNotExist;
    }

    switch(r.first - __orca__util__LatencyReplier_all)
    {
        case 0:
        {
            return ___ice_id(in, current);
        }
        case 1:
        {
            return ___ice_ids(in, current);
        }
        case 2:
        {
            return ___ice_isA(in, current);
        }
        case 3:
        {
            return ___ice_ping(in, current);
        }
        case 4:
        {
            return ___setPinger(in, current);
        }
        case 5:
        {
            return ___shutdown(in, current);
        }
        case 6:
        {
            return ___takeIt(in, current);
        }
        case 7:
        {
            return ___takeItAndReturn(in, current);
        }
    }

    assert(false);
    return ::IceInternal::DispatchOperationNotExist;
}

void
orca::util::LatencyReplier::__write(::IceInternal::BasicStream* __os) const
{
    __os->writeTypeId(ice_staticId());
    __os->startWriteSlice();
    __os->endWriteSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__write(__os);
#else
    ::Ice::Object::__write(__os);
#endif
}

void
orca::util::LatencyReplier::__read(::IceInternal::BasicStream* __is, bool __rid)
{
    if(__rid)
    {
        ::std::string myId;
        __is->readTypeId(myId);
    }
    __is->startReadSlice();
    __is->endReadSlice();
#if defined(_MSC_VER) && (_MSC_VER < 1300) // VC++ 6 compiler bug
    Object::__read(__is, true);
#else
    ::Ice::Object::__read(__is, true);
#endif
}

void
orca::util::LatencyReplier::__write(const ::Ice::OutputStreamPtr&) const
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type orca::util::LatencyReplier was not generated with stream support";
    throw ex;
}

void
orca::util::LatencyReplier::__read(const ::Ice::InputStreamPtr&, bool)
{
    Ice::MarshalException ex(__FILE__, __LINE__);
    ex.reason = "type orca::util::LatencyReplier was not generated with stream support";
    throw ex;
}

void 
orca::util::__patch__LatencyReplierPtr(void* __addr, ::Ice::ObjectPtr& v)
{
    ::orca::util::LatencyReplierPtr* p = static_cast< ::orca::util::LatencyReplierPtr*>(__addr);
    assert(p);
    *p = ::orca::util::LatencyReplierPtr::dynamicCast(v);
    if(v && !*p)
    {
        ::Ice::UnexpectedObjectException e(__FILE__, __LINE__);
        e.type = v->ice_id();
        e.expectedType = ::orca::util::LatencyReplier::ice_staticId();
        throw e;
    }
}

bool
orca::util::operator==(const ::orca::util::LatencyReplier& l, const ::orca::util::LatencyReplier& r)
{
    return static_cast<const ::Ice::Object&>(l) == static_cast<const ::Ice::Object&>(r);
}

bool
orca::util::operator!=(const ::orca::util::LatencyReplier& l, const ::orca::util::LatencyReplier& r)
{
    return static_cast<const ::Ice::Object&>(l) != static_cast<const ::Ice::Object&>(r);
}

bool
orca::util::operator<(const ::orca::util::LatencyReplier& l, const ::orca::util::LatencyReplier& r)
{
    return static_cast<const ::Ice::Object&>(l) < static_cast<const ::Ice::Object&>(r);
}

bool
orca::util::operator<=(const ::orca::util::LatencyReplier& l, const ::orca::util::LatencyReplier& r)
{
    return l < r || l == r;
}

bool
orca::util::operator>(const ::orca::util::LatencyReplier& l, const ::orca::util::LatencyReplier& r)
{
    return !(l < r) && !(l == r);
}

bool
orca::util::operator>=(const ::orca::util::LatencyReplier& l, const ::orca::util::LatencyReplier& r)
{
    return !(l < r);
}
