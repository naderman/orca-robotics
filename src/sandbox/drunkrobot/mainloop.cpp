/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Waleed Kadous, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <orcaice/orcaice.h>

#include "mainloop.h"

using namespace std;
using namespace drunk; 

MainLoop::MainLoop( hydroutil::Buffer<orca::Position2dData> &posBuffer,
                    const orca::Position2dConsumerPrx &position2dConsumer):
    posBuffer_(posBuffer), 
    position2dConsumer_(position2dConsumer)
{
    //Nothing to do. 
}

MainLoop::~MainLoop(){

}

void
MainLoop::run(){

    cout << "Ok, starting thread ..." << endl; 
    int msgCount = 0; 
    while( !isStopping())
    {

        // Generate a random position every time stemp. 
        orca::Position2dData currentPos;
        orcaice::setSane(currentPos); // Function from libOrcaIce that provides reasonable random values.

        cout << "Message: " << msgCount ++ << " Position is: " << orcaice::toString(currentPos) << endl; 

        // Next we send it out via the IceStorm system 

        try
        {
            position2dConsumer_->setData( currentPos );
        }
        catch ( const Ice::CommunicatorDestroyedException & e )
        {
            cout<<"The comm man is dead"<<endl;
            // it's ok, the communicator may already be destroyed
        }
    
        // Then we put it in the buffer for pull requests. 
        posBuffer_.push( currentPos );

        // And now, sleep for a little while. For platform independence, use 
        // Ice's sleep implementation. 
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    
    }

}
