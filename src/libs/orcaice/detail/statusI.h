/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_STATUS_I_H
#define ORCAICE_STATUS_I_H

#include <orca/status.h>
#include "localstatus.h"

#include <IceStorm/IceStorm.h>
#include <IceUtil/Mutex.h>

namespace orcaice
{
namespace detail
{

class SysLogger;

// An implementation of the (remote) Status interface.
class StatusI : public virtual orca::Status, public LocalStatus
{
public:
    
    StatusI( const orcaice::Context & context );

    virtual ~StatusI();

    // orca::Status interface

    virtual ::orca::StatusData getData(const ::Ice::Current& ) const;
    
    virtual void subscribe(const ::orca::StatusConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::StatusConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    // orcaice::Status interface

    // Routing is determined by HeartbeatToXxx parameter.
    virtual void heartbeat( const std::string &message, int level=1 );
    
    // Routing is determined by StatusToXxx parameter.
    virtual void status( const std::string &message, bool force=false );
    
    // Returns current status.
    virtual std::string status() const;

private:

//     Tracer::Config   config_;
    orcaice::Context context_;
    std::string prefix_;
    // We only have one communicator but may have multiple threads.
    IceUtil::Mutex mutex_;
    std::string prevWarning_;
    std::string prevError_;

    // Status stuff   
    IceStorm::TopicPrx topic_;
    orca::StatusConsumerPrx publisher_;
    bool connectToIceStorm();
    void icestormConnectFailed( const orca::FQTopicName &fqTName,
                                orca::StatusConsumerPrx &publisher,
                                bool isStatusTopicRequired );

    std::ofstream *file_;
    detail::SysLogger *sysLogger_;
};


} // namespace
} // namespace

#endif
