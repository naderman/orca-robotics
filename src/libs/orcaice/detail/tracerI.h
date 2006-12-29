/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_TRACER_I_H
#define ORCAICE_TRACER_I_H

#include <orca/tracer.h>
#include "localtracer.h"

#include <IceStorm/IceStorm.h>
#include <IceUtil/Mutex.h>

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

    virtual void setVerbosity( ::Ice::Int error, ::Ice::Int warn, ::Ice::Int info, ::Ice::Int debug, 
                                const ::Ice::Current& = ::Ice::Current());

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
    IceStorm::TopicPrx topic_;
    orca::TracerConsumerPrx publisher_;
    bool connectToIceStorm();
    void icestormConnectFailed( const orca::FQTopicName &fqTName,
                                orca::TracerConsumerPrx &publisher,
                                bool isStatusTopicRequired );
};

} // namespace
} // namespace

#endif
