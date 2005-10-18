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

#include "teleopcomponent.h"
#include "networkloop.h"
#include "inputloop.h"    

using namespace std;

TeleopComponent::TeleopComponent()
    : orcaiceutil::Component( "Teleop" )
{
}

TeleopComponent::~TeleopComponent()
{
    delete networkLoop_;
    delete inputLoop_;
}

int TeleopComponent::go()
{
    //
    // NETWORK
    //
    networkLoop_ = new NetworkLoop( &commandProxy_ );
    networkLoop_->setupComms( communicator() );
    networkLoop_->setupConfigs( communicator()->getProperties() );
    networkLoop_->start();

    //
    // HARDWARE
    //
    inputLoop_ = new InputLoop( this, &commandProxy_ );
    inputLoop_->setupConfigs( communicator()->getProperties() );
    inputLoop_->start();
    
    communicator()->waitForShutdown();

    stop();
    /*
    // do clean up if there was a Ctrl-C, otherwise the driver has cleaned up itself
    if ( interrupted() )  {
        cerr<< appName() << ": terminating..." << endl;
        fsm.interruptShutdown();
    } else {
        cout<<appName()<<": exiting cleanly. Good bye."<<endl;
    }
    */
    return 0;
}

void TeleopComponent::stop()
{
    cout<<"stopping component"<<endl;

    networkLoop_->stop();
    cout<<"joining network loop ..."<<endl;
    networkLoop_->getThreadControl().join();

    inputLoop_->stop();
    cout<<"joining input loop ... Hit any key please."<<endl;
    inputLoop_->getThreadControl().join();
}
/*
void TeleopComponent::activate()
{
    //
    // NETWORK
    //
    networkLoop_ = new NetworkLoop( &commandProxy_ );
    networkLoop_->setupComms( comm_ );
    networkLoop_->activate();

    //
    // HARDWARE
    //
    inputLoop_ = new KeyboardDriver( this, &commandProxy_ );
    inputLoop_->setup( comm_->getProperties() );

    inputLoop_->activate();

}

void TeleopComponent::humanDeactivate()
{
    netDriver_->deactivate();
    cout<<"joining network hwDriver_..."<<endl;
    netDriver_->getThreadControl().join();

    // hw driver is assumed to be stopped
}

void TeleopComponent::interruptDeactivate()
{
    netDriver_->deactivate();
    cout<<"joining network hwDriver_..."<<endl;
    netDriver_->getThreadControl().join();

    hwDriver_->deactivate();
    cout<<"joining input hwDriver_... Hit any key please."<<endl;
    hwDriver_->getThreadControl().join();
}
*/
