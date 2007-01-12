/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_LOCAL_STATUS_H
#define ORCAICE_LOCAL_STATUS_H

#include "../status.h"

#include <IceUtil/Mutex.h>
#include <map>

namespace orcaice
{
namespace detail
{

class LocalStatus : public orcaice::Status
{
public:
    LocalStatus( const orcaice::Context& context );
    virtual ~LocalStatus() {};
    
    // from orcaice::Status

    virtual void setHeartbeatInterval( const std::string& subsystem, double maxHeartbeatInterval );

    virtual void heartbeat( const std::string& subsystem );
    
    virtual void subsys( const std::string& subsystem, SubsystemStatusType type, const std::string& message );

    virtual IceUtil::Time startTime() const;

protected:

    // Not implemented; prevents accidental use.
    LocalStatus( const LocalStatus & );
    LocalStatus& operator= ( const LocalStatus & );

    struct SubsystemStatus
    {
        SubsystemStatusType type;
        std::string message;
        IceUtil::Time lastHeartbeat;
        double maxHeartbeatInterval;
    };
    typedef std::map<std::string,SubsystemStatus> SubsystemsStatus;

    SubsystemsStatus subsystems_;

    orcaice::Context context_;

    IceUtil::Time startTime_;

    // We only have one communicator but may have multiple threads.
    IceUtil::Mutex mutex_;
};

} // namespace
} // namespace

#endif
