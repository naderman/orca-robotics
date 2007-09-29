/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Waleed Kadous, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_DRUNK_ROBOT_POSITION2D_I_H
#define ORCA2_DRUNK_ROBOT_POSITION2D_I_H

// include provided interfaces
#include <orca/position2d.h>

// utilities
#include <orcaiceutil/buffer.h>
#include <orcaice/connectutils.h>

namespace drunk 
{

// This interface allows us to get information in and out of our 
// robot. It uses a pointer buffer (a nice thread safe class from the 
// libOrcaIce library) to allow us to handle things asynchronously.
// We also use a pointer for geometry, since it's unlikely the geometry 
// will change much. 

class Position2dI : public orca::Position2d
{
public:

    Position2dI( orcaiceutil::Buffer<orca::Position2dData> &posBuffer, 
                 orca::Position2dGeometry &geomBuffer, 
                 const IceStorm::TopicPrx &topic);
    //
    // implement remote operation defined in the interface
    //

    virtual ::orca::Position2dData getData(const ::Ice::Current& ) const;

    virtual ::orca::Position2dGeometry getGeometry(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::Position2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Position2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

private:

    // the driver will put the latest data into this proxy
    orcaiceutil::Buffer<orca::Position2dData> &posBuffer_;
    
    // the driver will use this member to return geometry when requested. 
    orca::Position2dGeometry& geomBuffer_; 

    // This is used to set up the topic for us to push information. 
    IceStorm::TopicPrx topic_;
};

} // namespace

#endif
