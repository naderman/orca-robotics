/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_STATUS_H
#define ORCAICE_STATUS_H

#include <IceUtil/Time.h>

namespace orcaice
{

/*!
 *  Local interface to component status.
 *
 *  @see Tracer
 */
class Status
{
public:

    enum SubsystemStatusType
    {
        Ok,
        Warning,
        Fault
    };
    
    virtual ~Status() {};

    //! Set maximum expected interval between heartbeats. When time since last heartbeat exceeds this,
    //! alarm is raised.
    virtual void setHeartbeatInterval( const std::string& subsystem, double maxHeartbeatInterval ) = 0;

    //! Record heartbeat from a subsystem
    virtual void heartbeat( const std::string& subsystem ) = 0;
    
    //! Set status information for a subsystem
    virtual void subsys( const std::string& subsystem, SubsystemStatusType type, const std::string& message ) = 0 ;

    //! The time when component was activated
    virtual IceUtil::Time startTime() const = 0;
};


} // namespace

#endif
