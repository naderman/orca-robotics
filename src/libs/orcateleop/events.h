/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef  EVENTS_H
#define  EVENTS_H

#include <hydroiceutil/eventqueue.h>

namespace orcateleop
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

class SentVelocityCommandEvent : public hydroiceutil::Event
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

class SentBicycleCommandEvent : public hydroiceutil::Event
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

class SentRepeatCommandEvent : public hydroiceutil::Event
{
public:
    SentRepeatCommandEvent() :
        Event( SentRepeatCommand ) {};
};

class FailedToSendCommandEvent : public hydroiceutil::Event
{
public:
    FailedToSendCommandEvent() :
        Event( FailedToSendCommand ) {};
};

class MixedCommandEvent : public hydroiceutil::Event
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

class IncrementCommandEvent : public hydroiceutil::Event
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

class RelativeCommandEvent : public hydroiceutil::Event
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


class TeleopEventQueueOptimizer : public hydroiceutil::EventQueueOptimizer
{
public:
    // this combine function adds the member variables of the two events
    virtual bool combine( hydroiceutil::EventPtr& existing, const hydroiceutil::EventPtr& extra );
};

} // namespace

#endif
