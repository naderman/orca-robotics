/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMU_COMPONENT_H
#define ORCA2_IMU_COMPONENT_H

#include <orcaice/component.h>
#include <orcaifaceimpl/odometry3diface.h>

namespace imu {

class ImuHandler;
class ImuDriver;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    virtual void start();
    virtual void stop();

private:

    ImuHandler *handler_;

    //
    // EXTERNAL PROVIDED INTERFACE
    //
    Ice::ObjectPtr imuObjPtr_;
    orcaifaceimpl::Odometry3dIfacePtr odometry3dIface_;

    // Ice::ObjectPtr position3dObjPtr_;

    //
    // HARDWARE INTERFACES
    //
    ImuDriver *hwDriver_;
};

} //namespace

#endif