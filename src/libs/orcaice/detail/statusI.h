/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_DETAIL_STATUS_I_H
#define ORCAICE_DETAIL_STATUS_I_H

#include <orca/status.h>
#include "localstatus.h"

#include <IceStorm/IceStorm.h>
#include <IceUtil/Mutex.h>

namespace orcaice
{
namespace detail
{

// An implementation of the (remote) Status interface.
class StatusI : public virtual orca::Status
{
public:
    
    StatusI( const orcaice::Context & context );

    virtual ~StatusI();

    // orca::Status interface: remote calls

    virtual ::orca::StatusData getData(const ::Ice::Current& ) const;
    
    virtual void subscribe(const ::orca::StatusConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::StatusConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Local Calls
    void localSetData( const std::map<std::string,LocalStatus::SubsystemStatus> &subsystemStatus );

private:

    bool connectToIceStorm();
    void icestormConnectFailed( const std::string &topicName,
                                orca::StatusConsumerPrx &publisher,
                                bool isStatusTopicRequired );

    void setStatusData( const std::map<std::string,LocalStatus::SubsystemStatus> &subsystemStatus );
    void sendToIceStorm( const orca::StatusData &statusData );

    // utilities
    void convert( const std::map<std::string,LocalStatus::SubsystemStatus> &internal,
                  orca::SubsystemsStatus &network ) const;

    IceStorm::TopicPrx topic_;
    orca::StatusConsumerPrx publisher_;
    std::string topicName_;
    bool isStatusTopicRequired_;
    mutable orca::StatusData statusData_;

    // Protect from simultaneous get/set of statusData_
    IceUtil::Mutex mutex_;

    IceUtil::Time startTime_;

    orcaice::Context context_;
};

} // namespace
} // namespace

#endif
