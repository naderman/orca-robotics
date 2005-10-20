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

#ifndef ORCA2_LASERMON_KEYBOARD_DRIVER_H
#define ORCA2_LASERMON_KEYBOARD_DRIVER_H

#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

#include <orcaiceutil/thread.h>
#include <orca/position2d.h>

class Position2dMonInput : public orcaiceutil::Thread
{
public:

    Position2dMonInput( const orca::Position2dPrx & pos, 
                        const IceStorm::TopicPrx & topic,
                        const Ice::ObjectPrx & callbackPrx  );
    virtual ~Position2dMonInput();

    void setup( const Ice::PropertiesPtr & );
    void setupCommunicator( const Ice::CommunicatorPtr & comm ) { comm_=comm; };

    void activate();
    void deactivate();

    virtual void run();


private:

    orca::Position2dPrx position2dPrx_;
    IceStorm::TopicPrx topic_;
    Ice::ObjectPrx callbackPrx_;

    void keyboardHelp();

    Ice::CommunicatorPtr comm_;
};

#endif
