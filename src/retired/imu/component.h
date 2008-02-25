/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#include <orcaice/component.h>
#include <orca/bros1.h>
#include <orcaifaceimpl/odometry3dImpl.h>

namespace imu {

class MainThread;
class ImuDriver;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    virtual void start();
    virtual void stop();

private:

    MainThread *mainThread_;

    //
    // EXTERNAL PROVIDED INTERFACE
    //
    Ice::ObjectPtr imuObjPtr_;
    orcaifaceimpl::Odometry3dImplPtr odometry3dImpl_;

    // Ice::ObjectPtr position3dObjPtr_;

    //
    // HARDWARE INTERFACES
    //
    ImuDriver *hwDriver_;
};

} //namespace

#endif