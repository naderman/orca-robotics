/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_EVENTS_H
#define ORCA2_TELEOP_EVENTS_H

#include <orcaice/eventqueue.h>

namespace teleop
{

enum EventType  
{
    // user & network
    SentNewVelocityCommand=0,
    SentNewBicycleCommand,
    SentRepeatCommand,
    FailedToSendCommand,
    // network
    NewCommandIncrement,
    NewRelativeCommand
};

class SentNewVelocityCommandEvent : public orcaice::Event
{
public:
    SentNewVelocityCommandEvent( double vx, double vy, double w, bool vxLimit, bool vyLimit, bool wLimit ) :
        Event( SentNewVelocityCommand ),
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
typedef IceUtil::Handle<SentNewVelocityCommandEvent> SentNewVelocityCommandEventPtr;

class SentRepeatCommandEvent : public orcaice::Event
{
public:
    SentRepeatCommandEvent() :
        Event( SentRepeatCommand ) {};
};

class FailedToSendCommandEvent : public orcaice::Event
{
public:
    FailedToSendCommandEvent() :
        Event( FailedToSendCommand ) {};
};

class NewCommandIncrementEvent : public orcaice::Event
{
public:
    NewCommandIncrementEvent( int longitudinal, int transverse, int angle ) :
        Event( NewCommandIncrement ),
        longitudinal_(longitudinal),
        transverse_(transverse),
        angle_(angle) {};

    int longitudinal_;
    int transverse_;
    int angle_;
};
typedef IceUtil::Handle<NewCommandIncrementEvent> NewCommandIncrementEventPtr;

class NewRelativeCommandEvent : public orcaice::Event
{
public:
    NewRelativeCommandEvent( double longitudinal, double transverse, double angle ) :
        Event( NewRelativeCommand ),
        longitudinal_(longitudinal),
        transverse_(transverse),
        angle_(angle) {};

    double longitudinal_;
    double transverse_;
    double angle_;
};
typedef IceUtil::Handle<NewRelativeCommandEvent> NewRelativeCommandEventPtr;

} // namespace

#endif
