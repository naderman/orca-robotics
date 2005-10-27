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

#ifndef ORCA2_SEGWAY_PLATFORM2D_I_H
#define ORCA2_SEGWAY_PLATFORM2D_I_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/platform2d.h>

// utilities
#include <orcaiceutil/ptrbuffer.h>
#include <orcaiceutil/ptrnotify.h>


// serves latest data on demand and accepts commands
class Platform2dI : public orca::Platform2d
{
public:
    Platform2dI( orcaiceutil::PtrBuffer<orca::Position2dDataPtr>    &position2d,
                 //orcaiceutil::PtrBuffer<orca::Velocity2dCommandPtr> &commands,
                 orcaiceutil::PtrNotify &commands,
                 orcaiceutil::PtrBuffer<orca::Platform2dConfigPtr>  &setConfigBuffer,
                 orcaiceutil::PtrBuffer<orca::Platform2dConfigPtr>  &currentConfigBuffer,
                 const IceStorm::TopicPrx &topic );

    virtual ::orca::Position2dDataPtr getData(const ::Ice::Current& ) const;

    virtual ::orca::Position2dGeometryPtr getGeometry(const ::Ice::Current& ) const;

    virtual ::orca::Platform2dConfigPtr getConfig(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::Position2dConsumerPrx&, ::Ice::Double, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Position2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    virtual void setCommand(const ::orca::Velocity2dCommandPtr&, const ::Ice::Current& );

    virtual void setConfig(const ::orca::Platform2dConfigPtr&, const ::Ice::Current& );

    // WARING: not implemented!
    virtual void enableMotor(bool, const ::Ice::Current& = ::Ice::Current()) {};


    // the driver will put the latest data into this proxy
    orcaiceutil::PtrBuffer<orca::Position2dDataPtr> &position2dProxy_;
    // the driver will take the latest command from the proxy
    //orcaiceutil::PtrBuffer<orca::Velocity2dCommandPtr> &commandProxy_;
    orcaiceutil::PtrNotify &commandNotify_;

    // for incoming requests
    orcaiceutil::PtrBuffer<orca::Platform2dConfigPtr> &setConfigBuffer_;
    // for the current config
    orcaiceutil::PtrBuffer<orca::Platform2dConfigPtr> &currentConfigBuffer_;

    IceStorm::TopicPrx topic_;
};


#endif
