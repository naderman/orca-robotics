/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_EVENTS_H
#define ORCA2_TELEOP_EVENTS_H

#include <hydroutil/eventqueue.h>

namespace teleop
{

enum EventType  
{
    // user & network
    SentVelocityCommand=0,
    SentBicycleCommand,
    SentRepeatCommand,
    FailedToSendCommand,
    // network
    MixedCommand,
    IncrementCommand,
    RelativeCommand
};

class SentVelocityCommandEvent : public hydroutil::Event
{
public:
    SentVelocityCommandEvent( double vx, double vy, double w, bool vxLimit, bool vyLimit, bool wLimit ) :
        Event( SentVelocityCommand ),
        vx_(vx),
        vy_(vy),
        w_(w),
        vxLimit_(vxLimit),
        vyLimit_(vyLimit),
        wLimit_(wLimit) {};

    double vx_;
    double vy_;
    double w_;
    bool vxLimit_;
    bool vyLimit_;
    bool wLimit_;
};
typedef IceUtil::Handle<SentVelocityCommandEvent> SentVelocityCommandEventPtr;

class SentBicycleCommandEvent : public hydroutil::Event
{
public:
    SentBicycleCommandEvent( double speed, double steerAngle, bool speedLimit, bool steerAngleLimit ) :
        Event( SentBicycleCommand ),
        speed_(speed),
        steerAngle_(steerAngle),
        speedLimit_(speedLimit),
        steerAngleLimit_(steerAngleLimit) {};

    double speed_;
    double steerAngle_;
    bool speedLimit_;
    bool steerAngleLimit_;
};
typedef IceUtil::Handle<SentBicycleCommandEvent> SentBicycleCommandEventPtr;

class SentRepeatCommandEvent : public hydroutil::Event
{
public:
    SentRepeatCommandEvent() :
        Event( SentRepeatCommand ) {};
};

class FailedToSendCommandEvent : public hydroutil::Event
{
public:
    FailedToSendCommandEvent() :
        Event( FailedToSendCommand ) {};
};

class MixedCommandEvent : public hydroutil::Event
{
public:
    MixedCommandEvent( double lon, bool isLong, double tr, bool isTr, double ang, bool isAng ) :
        Event( MixedCommand ),
        longitudinal(lon),
        isLongIncrement(isLong),
        transverse(tr),
        isTransverseIncrement(isTr),
        angular(ang),
        isAngularIncrement(isAng) {};

    double longitudinal;
    bool isLongIncrement;
    double transverse;
    bool isTransverseIncrement;
    double angular;
    bool isAngularIncrement;
};
typedef IceUtil::Handle<MixedCommandEvent> MixedCommandEventPtr;

class IncrementCommandEvent : public hydroutil::Event
{
public:
    IncrementCommandEvent( int lon, int tr, int ang ) :
        Event( IncrementCommand ),
        longitudinal(lon),
        transverse(tr),
        angular(ang) {};

    int longitudinal;
    int transverse;
    int angular;
};
typedef IceUtil::Handle<IncrementCommandEvent> IncrementCommandEventPtr;

class RelativeCommandEvent : public hydroutil::Event
{
public:
    RelativeCommandEvent( double lon, double tr, double ang ) :
        Event( RelativeCommand ),
        longitudinal(lon),
        transverse(tr),
        angular(ang) {};

    double longitudinal;
    double transverse;
    double angular;
};
typedef IceUtil::Handle<RelativeCommandEvent> RelativeCommandEventPtr;


class TeleopEventQueueOptimizer : public hydroutil::EventQueueOptimizer
{
public:
    // this combine function adds the member variables of the two events
    virtual bool combine( hydroutil::EventPtr& existing, const hydroutil::EventPtr& extra );
};

} // namespace

#endif
