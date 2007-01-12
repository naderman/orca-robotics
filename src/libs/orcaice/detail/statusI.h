/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

//     virtual void heartbeat( const std::string& subsystem );
//     
//     virtual void status( const std::string& subsystem, SubsystemStatusType type, const std::string& message );
// 
//     virtual IceUtil::Time startTime() const;

private:

    // Status stuff   
    IceStorm::TopicPrx topic_;
    orca::StatusConsumerPrx publisher_;
    bool connectToIceStorm();
    void icestormConnectFailed( const orca::FQTopicName &fqTName,
                                orca::StatusConsumerPrx &publisher,
                                bool isStatusTopicRequired );

    // utilities
    static void convert( const LocalStatus::SubsystemsStatus& internal, orca::SubsystemsStatus& network );
};


} // namespace
} // namespace

#endif
