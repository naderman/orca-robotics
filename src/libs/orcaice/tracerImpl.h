/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_TRACER_IMPL_H
#define ORCAICE_TRACER_IMPL_H

#include <hydroiceutil/localtracer.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>

#include <orca/tracer.h>
#include <orcaice/topichandler.h>

// class gbxiceutilacfr::Thread;

namespace orcaice
{

class TracerImpl : public hydroiceutil::LocalTracer, public IceUtil::Shared
{
friend class TracerI;

public:
    TracerImpl( const orcaice::Context & context );
    ~TracerImpl(); 

    void updateContext( const orcaice::Context& context ) { context_ = context; };
    void initInterface();
//     void initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

private:

    // from LocalTracer
    virtual void toNetwork( gbxutilacfr::TraceType traceType, const std::string& message, int level );

    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::TracerVerbosityConfig internalGetVerbosity();
    void internalSetVerbosity( const ::orca::TracerVerbosityConfig& config );

    void initTopicHandler();
    void icestormConnectFailed( const std::string &topicName, bool isTracerTopicRequired );
    // tracer service is special: it can be self-referencial, i.e. errors during initialization
    // will produce traces which will lead to more errors.
    // With this thread-safe flag, traces will not be published to the network until full initialization.
    gbxiceutilacfr::Store<bool> isInitialized_;

    typedef TopicHandler<orca::TracerConsumerPrx,orca::TracerData> TracerTopicHandler;

    // Responsible for sending messages to the component's tracer topic.
    TracerTopicHandler* topicHandler_;

    void internalSubscribe( const ::orca::TracerConsumerPrx &subscriber );
    void internalUnsubscribe( const ::orca::TracerConsumerPrx &subscriber );

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr ptr_;

    const std::string  interfaceName_;
    orcaice::Context context_;
};

typedef IceUtil::Handle<TracerImpl> TracerImplPtr;

} // namespace

#endif
