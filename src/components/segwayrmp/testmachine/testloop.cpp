/*
 *  Orca-Components: Components for robotics.
 *  
 *  Copyright (C) 2004
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>

#include "testloop.h"

#include <orcaiceutil/orcaiceutil.h>

using namespace std;
using namespace orca;
using orcaiceutil::operator<<;

TestLoop::TestLoop( const Position2dConsumerPrx position2dCallbackPrx,
                    const orca::PowerConsumerPrx powerCallbackPrx ) :
        position2dCallbackPrx_(position2dCallbackPrx),
        powerCallbackPrx_(powerCallbackPrx)
{
}

TestLoop::~TestLoop()
{
}

// read commands from the keyboard
void TestLoop::run()
{
    orca::Platform2dPrx platform2dPrx;
    orca::PowerPrx powerPrx;

    // REQUIRED : Position2d
    while ( isActive() ) {
        try
        {
            // create a proxy for the remote server based on its name in the config file
            orcaiceutil::connectToInterface<Platform2dPrx>( current_, platform2dPrx, "Platform2d" );
            current_.logger()->trace("remote","connected to a 'Platform2d' interface");
            break;
        }
        catch ( const Ice::ConnectionRefusedException & e )
        {
            //current_.logger()->trace("remote","failed to connect to a remote interface");
            sleep(2);
        }
    }

    /*
    orcaiceutil::connectToInterface<orca::PowerPrx>( current(), powerPrx, "Power" );
    */

    Position2dDataPtr position2dData = new Position2dData;
    Velocity2dCommandPtr velocity2dCommand = new Velocity2dCommand;

    while ( isActive() ) {
        if ( rand()%100 > 75 ) {
            platform2dPrx->subscribe( position2dCallbackPrx_, 2.0 );
        }

        if ( rand()%100 > 75 ) {
            platform2dPrx->unsubscribe( position2dCallbackPrx_ );
        }

        if ( rand()%100 > 75 ) {
            position2dData = platform2dPrx->getData();
        }

        if ( rand()%100 > 75 ) {
            orcaiceutil::setSane( velocity2dCommand );
            platform2dPrx->setCommand( velocity2dCommand );
        }

        //powerPrx->subscribe( powerCallbackPrx_, 2.0 );

        sleep( 2 );
    }
    //cout<<"exiting thread"<<endl;
}
