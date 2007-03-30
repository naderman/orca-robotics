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
#include "statusI.h"

using namespace std;

using namespace orcaice::detail;

LocalStatus::LocalStatus( const orcaice::Context& context,
                          StatusI *statusI )
    : context_(context),
      statusI_(statusI),
      statusTouched_(false)
{
    publishPeriodSec_ = getPropertyAsDoubleWithDefault( context_.properties(), "Orca.Status.PublishPeriod", 30 );
    lastPublishTime_ = IceUtil::Time::now();
}

LocalStatus::~LocalStatus()
{
}

void 
LocalStatus::setMaxHeartbeatInterval( const std::string& subsystem,
                                      double maxHeartbeatIntervalSec )
{
    IceUtil::Mutex::Lock lock(mutex_);
    subsystems_[subsystem].maxHeartbeatInterval = maxHeartbeatIntervalSec;
    subsystems_[subsystem].lastHeartbeatTime    = IceUtil::Time::now();
}

void 
LocalStatus::heartbeat( const std::string& subsystem )
{
    IceUtil::Mutex::Lock lock(mutex_);
    subsystems_[subsystem].lastHeartbeatTime = IceUtil::Time::now();
}

void 
LocalStatus::initialising( const std::string& subsystem, const std::string& message )
{
    setSubsystemStatus( subsystem, orcaice::Status::Initialising, message );
}

void 
LocalStatus::ok( const std::string& subsystem, const std::string& message )
{
    setSubsystemStatus( subsystem, orcaice::Status::Ok, message );
}

void 
LocalStatus::warning( const std::string& subsystem, const std::string& message )
{
    setSubsystemStatus( subsystem, orcaice::Status::Warning, message );
}

void 
LocalStatus::fault( const std::string& subsystem, const std::string& message )
{
    setSubsystemStatus( subsystem, orcaice::Status::Fault, message );
}
    
void 
LocalStatus::setSubsystemStatus( const std::string& subsystem,
                                 SubsystemStatusType type,
                                 const std::string& message )
{
    IceUtil::Mutex::Lock lock(mutex_);

    std::map<std::string,SubsystemStatus>::iterator it;
    it = subsystems_.find( subsystem );

    if ( it == subsystems_.end() )
    {
        stringstream ss;
        ss << "LocalStatus: Haven't previously heard of subsystem '"<<subsystem<<"' -- ignoring.";
        context_.tracer()->warning( ss.str() );
        return;
    }

    if ( !statusTouched_ )
    {
        statusTouched_ = ( type != subsystems_[subsystem].type ||
                           message != subsystems_[subsystem].message );
    }

    it->second.type = type;
    it->second.message = message;
    it->second.lastHeartbeatTime = IceUtil::Time::now();
}

void 
LocalStatus::process()
{
    IceUtil::Mutex::Lock lock(mutex_);

    if ( subsystems_.size() == 0 )
    {
        // No subsystems have been defined -- no status to report.
        return;
    }

    IceUtil::Time now = IceUtil::Time::now();
    std::map<std::string,SubsystemStatus>::iterator it;
    for ( it=subsystems_.begin(); it!=subsystems_.end(); ++it ) 
    {
        IceUtil::Time timeSinceLastHeartbeat = now-it->second.lastHeartbeatTime;
        double secSinceLastHeartbeat = timeSinceLastHeartbeat.toSeconds();
        if ( secSinceLastHeartbeat > it->second.maxHeartbeatInterval )
        {
            // Oops, this subsystem appears to be dead...
            it->second.type = orcaice::Status::Stalled;
            stringstream ss; 
            ss << "Subsystem hasn't been heard from for "<<secSinceLastHeartbeat<<"s.";
            it->second.message = ss.str();
            statusTouched_ = true;
        }
    }

    IceUtil::Time timeSinceLastPublish = now-lastPublishTime_;
    bool isPublishTime = (timeSinceLastPublish.toSeconds() >= publishPeriodSec_);

    if ( !statusTouched_ && !isPublishTime ) return;
    statusTouched_ = false;

    if ( statusI_ != NULL )
    {
        statusI_->localSetData( subsystems_ );
        lastPublishTime_ = now;
    }
}
