/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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

#ifndef ORCA2_SEGWAY_PLATFORM_I_H
#define ORCA2_SEGWAY_PLATFORM_I_H

// include defnition of Ice runtime
#include <Ice/Ice.h>

// include provided interfaces
#include <orca/platform2d.h>

// utilities
#include <orcaiceutil/ptrproxy.h>


// serves latest data on demand and accepts commands
class Platform2dI : public orca::Platform2d
{
public:
    Platform2dI( orcaiceutil::PtrProxy* position2d, orcaiceutil::PtrProxy* commands );

    virtual ::orca::Position2dDataPtr getData(const ::Ice::Current& ) const;

    virtual ::orca::Position2dGeometryPtr getGeometry(const ::Ice::Current& ) const;

    virtual void putData(const ::orca::Velocity2dCommandPtr&, const ::Ice::Current& );

    virtual void enableMotor(bool, const ::Ice::Current& = ::Ice::Current()) {};

    // the driver will put the latest data into this proxy
    orcaiceutil::PtrProxy* position2d_;
    // the driver will take the latest command from the proxy
    orcaiceutil::PtrProxy* command_;
};


#endif
