/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Waleed Kadous, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <orcaice/orcaice.h>

#include "drunkrobotmainloop.h"

using namespace std;
using namespace orca;
using namespace orcaice; 

DrunkRobotMainLoop::DrunkRobotMainLoop(PtrBuffer<Position2dDataPtr> &posBuffer,
				       const Position2dConsumerPrx &position2dConsumer):
  posBuffer_(posBuffer), position2dConsumer_(position2dConsumer)
{
  //Nothing to do. 
}

DrunkRobotMainLoop::~DrunkRobotMainLoop(){

}

void
DrunkRobotMainLoop::run(){

  cout << "Ok, starting thread ..." << endl; 
  int msgCount = 0; 
  while( isActive())
  {

    // Generate a random position every time stemp. 
   Position2dDataPtr currentPos = new Position2dData;
   setSane(currentPos); // Function from libOrcaIce that provides reasonable random values.

    cout << "Message: " << msgCount ++ << " Position is: " << currentPos << endl; 

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
