/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_TRACER_I_H
#define ORCAICE_TRACER_I_H

#include <orca/tracer.h>
#include "localtracer.h"
#include "networktracesender.h"
#include <IceStorm/IceStorm.h>

namespace orcaice
{
namespace detail
{

class TracerI : public virtual orca::Tracer, public LocalTracer
{
public:
    TracerI( const orcaice::Context & context );
    virtual ~TracerI(); 

    // orca::Tracer interface

    virtual ::orca::TracerVerbosityConfig getVerbosity( const ::Ice::Current& = ::Ice::Current()) const;

    virtual void setVerbosity( const ::orca::TracerVerbosityConfig&, const ::Ice::Current& = ::Ice::Current());

    virtual void subscribe(const ::orca::TracerConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::TracerConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // orcaice::Tracer interface
    // reimplement from LocalTracer because we are adding toNetwork() option

    virtual void info( const std::string &message, int level=1 );
    
    virtual void warning( const std::string &message, int level=1 );
    
    virtual void error( const std::string &message, int level=1 );

    virtual void debug( const std::string &message, int level=1 );

private:

    // Not implemented; prevents accidental use.
    TracerI( const TracerI & );
    TracerI& operator= ( const TracerI & );

    // to network
    void toNetwork( const std::string& category, const std::string& message, int level );
    void setupAndConnectNetworkSenders();
    void icestormConnectFailed( const std::string &topicName,
                                bool isTracerTopicRequired );

//     IceStorm::TopicPrx topic_;
//     orca::TracerConsumerPrx publisher_;
//     std::vector<NetworkTraceSender*> networkTraceSenders_;

    // Responsible for sending messages to the component's tracer topic
    NetworkTraceSender *componentTraceSender_;

};

} // namespace
} // namespace

#endif
