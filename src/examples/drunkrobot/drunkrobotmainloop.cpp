/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
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
#include <iostream>
#include "drunkrobotmainloop.h"
#include <orcaiceutil/objutils.h>

using namespace std;
using namespace orca;
using namespace orcaiceutil; 

DrunkRobotMainLoop::DrunkRobotMainLoop(PtrBuffer<Position2dDataPtr> &posBuffer,
				       const Position2dConsumerPrx &position2dConsumer):
  posBuffer_(posBuffer), position2dConsumer_(position2dConsumer)
{
  //Nothing to do. 
}

DrunkRobotMainLoop::~DrunkRobotMainLoop(){

}

void DrunkRobotMainLoop::run(){

  cout << "Ok, starting thread ..." << endl; 
  int msgCount = 0; 
  while( isActive())
  {

    // Generate a random position every time stemp. 
   Position2dDataPtr currentPos = new Position2dData;
   setSane(currentPos); // Function from orcaiceutil tha provides reasonable random values. 

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
