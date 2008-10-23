/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaifacestring/status.h>
#include "statusconsumerImpl.h"

using namespace std;
namespace statusmon {

StatusConsumerImpl::StatusConsumerImpl( const Config& config, const orcaice::Context& context ) :
    orcaifaceimpl::ConsumerImpl<orca::StatusPrx,orca::StatusConsumer,orca::StatusConsumerPrx,orca::StatusData>(context),
    hasValidData_(false),
    isDestructing_(false),
    config_(config)
{
}

StatusConsumerImpl::~StatusConsumerImpl()
{
    IceUtil::Mutex::Lock lock(statusMutex_);
    isDestructing_ = true;

    // it would be nice to unsubscribe when destructing but ...
    // this would be a remote call which would need to be done as a job.
    // the extra complexity does not seem necessary, we'll just let IceStorm
    // figure it out and drop us on the next data push.
}

void
StatusConsumerImpl::init()
{
    if ( isDestructing_ ) {
        return;
    }

    std::string proxyStr = string("status@")+config_.platformName+"/"+config_.componentName;
    subscribeWithString( proxyStr );

    IceUtil::Mutex::Lock lock(statusMutex_);

    lastStatusTime_      = IceUtil::Time::now();
    lastResubscribeTime_ = IceUtil::Time::now();
}

void
StatusConsumerImpl::dataEvent( const orca::StatusData& data ) 
{
    IceUtil::Mutex::Lock lock(statusMutex_);

    statusData_ = data;
    hasValidData_ = true;
    lastStatusTime_ = IceUtil::Time::now();
}

StatusDetails
StatusConsumerImpl::getStatus()
{
    IceUtil::Mutex::Lock lock(statusMutex_);

    StatusDetails details;
    details.isStale = false;
    details.dataAvailable = hasValidData_;

    // alexm: why would this throw?
    try {
        details.statusData = statusData_;

        IceUtil::Time timeSinceLastUpdate = IceUtil::Time::now() - lastStatusTime_;
        details.secSinceHeard = timeSinceLastUpdate.toSeconds();
    
        if ( details.secSinceHeard > config_.resubscribeTimeout )
        {
            IceUtil::Time timeSinceLastResubscribe = IceUtil::Time::now() - lastResubscribeTime_;
            if ( timeSinceLastResubscribe.toSeconds() > config_.resubscribeInterval )
            {
                stringstream ss;
                ss << "StatusConsumerI("<<config_.platformName<<"/"<<config_.componentName<<"): Haven't heard from Status for "<<timeSinceLastUpdate << " -- resubscribing.";
                context_.tracer().warning( ss.str() );

                try {
                    std::string proxyStr = string("status@")+config_.platformName+"/"+config_.componentName;
                    subscribeWithString( proxyStr );
                    lastResubscribeTime_ = IceUtil::Time::now();
                }
                catch ( const std::exception &e )
                {
                    stringstream ssErr;
                    ssErr << "StatusConsumerI("<<config_.platformName<<"/"<<config_.componentName<<"): Failed to subscribe to status interface: " << e.what();
                    context_.tracer().warning( ssErr.str() );
                }
            }

            if ( details.secSinceHeard > config_.staleTimeout )
            {
                details.isStale = true;
            }
        }
        return details;
    }
    catch ( const std::exception &e )
    {
        stringstream ss;
        ss << "StatusConsumerI::getStatus(): Unexpected exception: " << e.what();
        context_.tracer().error( ss.str() );
        throw;        
    }
}

} // namespace
