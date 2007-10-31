/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_TRACER_I_H
#define ORCAICE_TRACER_I_H

#include <orca/tracer.h>
#include <hydroutil/localtracer.h>
#include "networktracesender.h"
#include <IceStorm/IceStorm.h>

namespace orcaice
{
namespace detail
{

class TracerI : public virtual orca::Tracer, public hydroutil::LocalTracer
{
public:
    TracerI( const orcaice::Context & context );
    virtual ~TracerI(); 

    // orca::Tracer interface

    virtual ::orca::TracerVerbosityConfig getVerbosity( const ::Ice::Current& = ::Ice::Current()) const;

    virtual void setVerbosity( const ::orca::TracerVerbosityConfig&, const ::Ice::Current& = ::Ice::Current());

    // subscribe/unsubscribe functions.
    virtual void subscribeForComponentMessages(const ::orca::TracerConsumerPrx &subscriber,
                                               const ::Ice::Current& = ::Ice::Current())
        { subscribe( componentTraceSender_, subscriber ); }
    virtual void unsubscribeForComponentMessages(const ::orca::TracerConsumerPrx &subscriber,
                                                 const ::Ice::Current& = ::Ice::Current())
        { unsubscribe( componentTraceSender_, subscriber ); }
    virtual void subscribeForPlatformInfoMessages(const ::orca::TracerConsumerPrx &subscriber,
                                                  const ::Ice::Current& = ::Ice::Current())
        { subscribe( platformInfoSender_, subscriber ); }
    virtual void unsubscribeForPlatformInfoMessages(const ::orca::TracerConsumerPrx &subscriber,
                                                    const ::Ice::Current& = ::Ice::Current())
        { unsubscribe( platformInfoSender_, subscriber ); }
    virtual void subscribeForPlatformWarningMessages(const ::orca::TracerConsumerPrx &subscriber,
                                                     const ::Ice::Current& = ::Ice::Current())
        { subscribe( platformWarningSender_, subscriber ); }
    virtual void unsubscribeForPlatformWarningMessages(const ::orca::TracerConsumerPrx &subscriber,
                                                       const ::Ice::Current& = ::Ice::Current())
        { unsubscribe( platformWarningSender_, subscriber ); }
    virtual void subscribeForPlatformErrorMessages(const ::orca::TracerConsumerPrx &subscriber,
                                                   const ::Ice::Current& = ::Ice::Current())
        { subscribe( platformErrorSender_, subscriber ); }
    virtual void unsubscribeForPlatformErrorMessages(const ::orca::TracerConsumerPrx &subscriber,
                                                     const ::Ice::Current& = ::Ice::Current())
        { unsubscribe( platformErrorSender_, subscriber ); }

    // hydroutil::Tracer interface
    // reimplement from LocalTracer because we are adding toNetwork() option

    virtual void info( const std::string &message, int level=1 );
    
    virtual void warning( const std::string &message, int level=1 );
    
    virtual void error( const std::string &message, int level=1 );

    virtual void debug( const std::string &message, int level=1 );

private:

    void subscribe( NetworkTraceSender *&sender, const ::orca::TracerConsumerPrx &subscriber );
    void unsubscribe( NetworkTraceSender *sender, const ::orca::TracerConsumerPrx &subscriber );

    // to network
    void toNetwork( hydroutil::Tracer::TraceType traceType,
                const std::string& message,
                int level );
    void setupAndConnectNetworkSenders();
    void trySetupNetworkTraceSender( NetworkTraceSender *&sender,
                                     std::string topic,
                                     bool isTracerTopicRequired );
    void icestormConnectFailed( const std::string &topicName,
                                bool isTracerTopicRequired );
    std::string categoryToString( hydroutil::Tracer::TraceType category );

    // Responsible for sending messages to the component's tracer topic
    NetworkTraceSender *componentTraceSender_;

    // Responsible for sending messages to the platform's info/warning/error topics
    NetworkTraceSender *platformInfoSender_;
    NetworkTraceSender *platformWarningSender_;
    NetworkTraceSender *platformErrorSender_;

    orcaice::Context context_;
};

} // namespace
} // namespace

#endif
