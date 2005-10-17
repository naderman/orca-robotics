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

#include "lasermoninput.h"

#include <orcaiceutil/objutils.h>

using namespace std;
using namespace orca;
using namespace orcaiceutil;

Position2dMonInput::Position2dMonInput( const Position2dPrx & pos, const IceStorm::TopicPrx & topic,
            const Ice::ObjectPrx & callbackPrx ) :
        position2dPrx_(pos), topic_(topic), callbackPrx_(callbackPrx)
{
}

Position2dMonInput::~Position2dMonInput()
{
}

void Position2dMonInput::setup( const Ice::PropertiesPtr & properties )
{
}

void Position2dMonInput::activate()
{
    cout<<"activating device"<<endl;
    start();
}

void Position2dMonInput::deactivate()
{
    cout<<"deactivating device"<<endl;

    // tell the driver thread to stop.
    // unfortunately, 'cin' in run() is blocked on getting user input
    // don't know how to make get out of it.
    stop();
}

// read commands from the keyboard
void Position2dMonInput::run()
{
    keyboardHelp();
    
    char c;

    while( cin.good() && isActive() )
    {
        try
        {
            cout << "==> ";
            cin >> c;
            if(c == 'g')
            {
                Position2dDataPtr data = position2dPrx_->getData();
                cout<<"got: "<<data<<endl;
            }
            else if(c == 's')
            {
                // Set the requested quality of service "reliability" =
                // "batch". This tells IceStorm to send events to the subscriber
                // in batches at regular intervals.
                IceStorm::QoS qos;
                qos["reliability"] = "batch";

                topic_->subscribe( qos, callbackPrx_ );
            }
            else if(c == 'u')
            {
                topic_->unsubscribe( callbackPrx_ );
            }
            else if(c == '?')
            {
                keyboardHelp();
            }
            else if (c=='x')
            {
                break;
            }
            else
            {
                cout<<endl<<"==> ";
            }
        }
        catch(const Ice::Exception& ex)
        {
            cerr << ex << endl;
        }
    }

    cout<<"Cleaned up, shutting down the communicator..."<<endl;
    try {
        comm_->shutdown();
    }
    catch ( const Ice::CommunicatorDestroyedException & e ) {
        // it's ok, if this is a Ctrl-C event, the communicator is already destroyed
    }
}

void Position2dMonInput::keyboardHelp()
{
    cout <<
        "usage:\n"
        "p: push data\t\tClientPush\n"
        "g: get data\t\tClientPull (blocking)\n"
//        "r: request data\t\tClientPull (non-blocking)\n"
        "s: subscribe\t\tServerPush\n"
        "u: usibscribe\n"
//        "q: shutdown server\n"
        "x: exit\n"
        "?: help\n";
}
