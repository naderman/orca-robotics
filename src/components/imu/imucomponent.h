/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006 Ben Upcroft
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_IMU_COMPONENT_H
#define ORCA2_IMU_COMPONENT_H

#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>
#include <orca/bros1.h>
#include <orca/imu.h>

class ImuHandler;
class ImuDriver;

class ImuComponent : public orcaice::Component
{
public:

    ImuComponent();
    virtual ~ImuComponent();

    virtual void start();
    virtual void stop();

private:

    ImuHandler *handler_;
    orca::ImuConfigDataPtr config_;

    //
    // EXTERNAL INTERFACE: Imu
    //
    Ice::ObjectPtr imuObjPtr_;

    //
    // HARDWARE INTERFACES
    //
    ImuDriver *hwDriver_;
};

#endif
