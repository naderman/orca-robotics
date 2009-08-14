/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROICEUTIL_DETAIL_LOCAL_STATUS_H
#define HYDROICEUTIL_DETAIL_LOCAL_STATUS_H

#include <gbxutilacfr/status.h>
#include <gbxutilacfr/tracer.h>
#include <hydroutil/properties.h>
#include <hydroutil/uncopyable.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

#include <IceUtil/Mutex.h>
#include <map>

namespace hydroiceutil
{

struct LocalSubsystemStatus
{
    LocalSubsystemStatus() :
        state(gbxutilacfr::SubsystemIdle),
        health(gbxutilacfr::SubsystemOk),
        isStalled(false),
        type(gbxutilacfr::SubsystemStandard),
        maxHeartbeatInterval(-1)
    {};

    gbxutilacfr::SubsystemState state;
    gbxutilacfr::SubsystemHealth health;
    std::string message;
    bool isStalled;

    gbxutilacfr::SubsystemType type;
    double maxHeartbeatInterval;
    gbxiceutilacfr::Timer heartbeatTimer;
};

typedef std::map<std::string,LocalSubsystemStatus> LocalSubsystemStatusMap;

struct LocalComponentStatus
{
    LocalComponentStatus() :
        infrastructure(gbxutilacfr::SubsystemIdle),
        publishIntervalSec(-1)
    {};
        
    gbxutilacfr::SubsystemState infrastructure;
    LocalSubsystemStatusMap subsystems;
    double publishIntervalSec;
};

/*!
Implementation of gbxutilacfr::Status.
All public functions are thread safe.

How do we determine when a subsystem is declared stalled?
The delay D is tolerated as long as D < ( M*H + A ) , where
- H is the heartbeat interval promissed by the subsystem
- M is multiplicative adjustment
- A is additive adjustment
*/
class LocalStatus : public gbxutilacfr::Status,
                    public hydroutil::Uncopyable
{
public:
    LocalStatus( gbxutilacfr::Tracer& tracer, const hydroutil::Properties& props );
    
    // from gbxutilacfr::Status

    // addSubsystem() must be called before notifcations of the subsytem's status.
    virtual void addSubsystem( const std::string& subsystem,
            double maxHeartbeatIntervalSec=-1.0, gbxutilacfr::SubsystemType type=gbxutilacfr::SubsystemStandard );
    virtual void removeSubsystem( const std::string& subsystem );
    virtual std::vector<std::string> subsystems();
    virtual gbxutilacfr::SubsystemStatus subsystemStatus( const std::string& subsystem );
    virtual gbxutilacfr::SubsystemState infrastructureState();
    virtual void setMaxHeartbeatInterval( const std::string& subsystem, double interval );
    virtual void setSubsystemType( const std::string& subsystem, gbxutilacfr::SubsystemType type );

    virtual void setSubsystemStatus( const std::string& subsystem, gbxutilacfr::SubsystemState state, 
            gbxutilacfr::SubsystemHealth health, const std::string& msg="" );

    virtual void initialising( const std::string& subsystem, const std::string& msg="" );
    virtual void working( const std::string& subsystem, const std::string& msg="" );
    virtual void finalising( const std::string& subsystem, const std::string& msg="" );

    virtual void ok( const std::string& subsystem, const std::string& msg="" );
    virtual void warning( const std::string& subsystem, const std::string& msg );
    virtual void fault( const std::string& subsystem, const std::string& msg );
    virtual void heartbeat( const std::string& subsystem );
    virtual void message( const std::string& subsystem, const std::string& msg );

    virtual void infrastructureInitialising();
    virtual void infrastructureWorking();
    virtual void infrastructureFinalising();
    virtual void process();

private:

    //! This event is called when 
    //! - the status of any of the registered subsystems changes (including change in the message)
    //! - the timer for periodic updates expires
    //!
    //! This implementation does nothing. Reimplement this function to do something useful.
    virtual void publishEvent( const LocalComponentStatus& componentStatus );

    LocalComponentStatus componentStatus_;

    // makes acess to to componentStatus_ thread-safe
    IceUtil::Mutex localStatusMutex_;

    // this utility function aquires mutex when called from initialising(), ok(), warning(), fault()
    void internalSetStatus( const std::string& subsystem, gbxutilacfr::SubsystemState state, 
                gbxutilacfr::SubsystemHealth health, const std::string& msg );

    void localPublish();

    gbxutilacfr::Tracer& tracer_;

    double toleranceMultiplicative_;
    double toleranceAdditive_;

    gbxiceutilacfr::Timer publishTimer_;
};

} // namespace

#endif
