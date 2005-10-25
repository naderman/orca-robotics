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

#ifndef ORCA2_TELEOP_NETWORK_LOOP_H
#define ORCA2_TELEOP_NETWORK_LOOP_H

#include <orcaiceutil/thread.h>
#include <orcaiceutil/current.h>
#include <orcaiceutil/ptrbuffer.h>

#include <orca/platform2d.h>


class NetworkLoop : public orcaiceutil::Thread
{
public:

    NetworkLoop( orcaiceutil::PtrBuffer<orca::Velocity2dCommandPtr> *commandBuffer );
    ~NetworkLoop();

    void setCurrent( const orcaiceutil::Current & current ) { current_=current; };

    void setupConfigs( const Ice::PropertiesPtr & properties );

    virtual void run();

private:

    int connect();

    // remote object
    orca::Platform2dPrx platform2dPrx_;

    // network/driver interface
    orcaiceutil::PtrBuffer<orca::Velocity2dCommandPtr>* commandBuffer_;

    int timeoutMs_;

    // component current context
    orcaiceutil::Current current_;

};

#endif
