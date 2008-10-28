/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef STATUS_CONSUMER_IMPL_H
#define STATUS_CONSUMER_IMPL_H

#include <orcaice/context.h>
#include <orcaifaceimpl/consumerImpl.h>
#include <orca/status.h>
#include <IceUtil/Mutex.h>
#include <string>

namespace systemstatus {

struct StatusDetails {
    
    // Have we ever received any status data?
    bool dataAvailable;

    // Does we consider status to have failed sending frequently enough?
    bool isDataStale;

    // The actual statusData object (only filled in if dataAvailable)
    orca::StatusData data;
    
};

//
// Listens to component's Status interface
//
class StatusConsumerImpl : 
        public orcaifaceimpl::ConsumerImpl<orca::StatusPrx,orca::StatusConsumer,orca::StatusConsumerPrx,orca::StatusData>
{
public: 

    struct Config
    {
        Config( const std::string& platform, 
                const std::string& comp, 
                int resubTimeout = 45, 
                int resubInterval = 5, 
                int staleTime = 60 ) : 
            platformName(platform),
            componentName(comp),
            resubscribeTimeout(resubTimeout),
            resubscribeInterval(resubInterval),
            staleTimeout(staleTime) {}
        std::string platformName;
        std::string componentName;
        // If we don't hear for longer than this timeout [sec], caller should try to resubscribe.
        int resubscribeTimeout;
        // Don't try to resubscribe more frequently than this interval [sec]
        int resubscribeInterval;
        // If we don't hear for longer than this timeout [sec], data is considered stale.
        int staleTimeout;
    };

    // Connects, subscribes for updates, and pulls data.
    // Throws exceptions if _anything_ goes wrong.
    //
    StatusConsumerImpl( const Config& config, const orcaice::Context& context );
    
    // Thread-safe (has to be because this object can be destroyed at any time)
    ~StatusConsumerImpl();

    // from orcaifaceimpl::ConsumerImpl
    virtual void dataEvent( const orca::StatusData& data );

    // Thread-safe. Makes a remote call, you may consider using a Job to call this fct.
    void subscribe();

    // Non-blocking, thread-safe, local call.
    // Returns true if the caller should resubscribe.
    bool getStatus( StatusDetails &details );

private: 
    
    IceUtil::Mutex  statusMutex_;
    orca::StatusData statusData_;

    bool hasValidData_;

    IceUtil::Time    lastDataReceivedTime_;
    IceUtil::Time    lastResubscribeTime_;

    // this state is needed because the base class destructor needs access to protected structures
    // but can't lock our mutex.
    bool isDestructing_;

    Config config_;
};

typedef IceUtil::Handle<StatusConsumerImpl> StatusConsumerImplPtr;

}

#endif
