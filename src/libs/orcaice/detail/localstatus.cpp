/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

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
}

LocalStatus::~LocalStatus()
{
}

void 
LocalStatus::setMaxHeartbeatInterval( const std::string& subsystem,
                                      double maxHeartbeatIntervalSec )
{
    IceUtil::Mutex::Lock lock(mutex_);

    std::map<std::string,SubsystemStatus>::iterator it;
    it = subsystems_.find( subsystem );

    if ( it == subsystems_.end() )
    {
        // Adding new subsytem
        stringstream ss;
        ss << "LocalStatus::setMaxHeartbeatInterval(): Adding new subsystem: '"<<subsystem<<"'";
        context_.tracer()->debug( ss.str() );

        // the timer is initialised to current time
        SubsystemStatus newStatus;
        newStatus.maxHeartbeatInterval = maxHeartbeatIntervalSec;
        newStatus.message = "";
        newStatus.type = hydroutil::Status::Initialising;
        subsystems_[subsystem] = newStatus;        
    }
    else
    {
        // Modifying previously-registered subsytem
        SubsystemStatus &status = it->second;
        status.maxHeartbeatInterval = maxHeartbeatIntervalSec;
        status.heartbeatTimer.restart();
    }
}

void 
LocalStatus::heartbeat( const std::string& subsystem )
{
    IceUtil::Mutex::Lock lock(mutex_);

    std::map<std::string,SubsystemStatus>::iterator it;
    it = subsystems_.find( subsystem );

    if ( it == subsystems_.end() )
    {
        stringstream ss;
        ss << "LocalStatus::heartbeat(): Haven't previously heard of subsystem '"<<subsystem<<"' -- ignoring.";
        context_.tracer()->warning( ss.str() );
        return;
    }

    subsystems_[subsystem].heartbeatTimer.restart();
}

void 
LocalStatus::initialising( const std::string& subsystem, const std::string& message )
{
    setSubsystemStatus( subsystem, hydroutil::Status::Initialising, message );
}

void 
LocalStatus::ok( const std::string& subsystem, const std::string& message )
{
    setSubsystemStatus( subsystem, hydroutil::Status::Ok, message );
}

void 
LocalStatus::warning( const std::string& subsystem, const std::string& message )
{
    setSubsystemStatus( subsystem, hydroutil::Status::Warning, message );
}

void 
LocalStatus::fault( const std::string& subsystem, const std::string& message )
{
    setSubsystemStatus( subsystem, hydroutil::Status::Fault, message );
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
    it->second.heartbeatTimer.restart();
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

    std::map<std::string,SubsystemStatus>::iterator it;
    for ( it=subsystems_.begin(); it!=subsystems_.end(); ++it ) 
    {
        if ( it->second.heartbeatTimer.elapsedSec() > it->second.maxHeartbeatInterval )
        {
            // Oops, this subsystem appears to be dead...
            it->second.type = hydroutil::Status::Stalled;
            stringstream ss; 
            ss << "Subsystem hasn't been heard from for "<<it->second.heartbeatTimer.elapsedSec()<<"s.";
            it->second.message = ss.str();
            statusTouched_ = true;
        }
    }

    bool isPublishTime = ( publishTimer_.elapsedSec() >= publishPeriodSec_ );

    if ( !statusTouched_ && !isPublishTime ) return;
    statusTouched_ = false;

    if ( statusI_ != NULL )
    {
        statusI_->localSetData( subsystems_ );
        publishTimer_.restart();
    }
}
