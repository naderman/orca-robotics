/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <IceUtil/Time.h>
#include <orcaice/orcaice.h>

#include "localstatus.h"

using namespace std;

using namespace orcaice::detail;

LocalStatus::LocalStatus( const orcaice::Context& context ) : 
    context_(context)
{
    startTime_ = IceUtil::Time::now();
}

void 
LocalStatus::setHeartbeatInterval( const std::string& subsystem, double maxHeartbeatInterval )
{
    IceUtil::Mutex::Lock lock(mutex_);

    subsystems_[subsystem].maxHeartbeatInterval = maxHeartbeatInterval;
}

void 
LocalStatus::heartbeat( const std::string& subsystem )
{
    IceUtil::Mutex::Lock lock(mutex_);

    subsystems_[subsystem].lastHeartbeat = IceUtil::Time::now();
}

void 
LocalStatus::ok( const std::string& subsystem, const std::string& message )
{
    subsystemStatus( subsystem, orcaice::Status::Ok, message );
}

void 
LocalStatus::warning( const std::string& subsystem, const std::string& message )
{
    subsystemStatus( subsystem, orcaice::Status::Warning, message );
}

void 
LocalStatus::fault( const std::string& subsystem, const std::string& message )
{
    subsystemStatus( subsystem, orcaice::Status::Fault, message );
}
    
void 
LocalStatus::subsystemStatus( const std::string& subsystem, SubsystemStatusType type, const std::string& message )
{
    IceUtil::Mutex::Lock lock(mutex_);

    subsystems_[subsystem].type = Status::Ok;
    subsystems_[subsystem].message = message;
    subsystems_[subsystem].lastHeartbeat = IceUtil::Time::now();
}

IceUtil::Time 
LocalStatus::startTime() const
{
    return startTime_;
}
