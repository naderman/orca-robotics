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

#include <orcaice/orcaice.h>

// Include headers for the main loop and components. 

#include "drunkrobotcomponent.h"
#include "drunkrobotmainloop.h"

//Also include headers for position2dI.h

#include "position2dI.h"

using namespace std;
using namespace orca;
using namespace orcaice; 

DrunkRobotComponent::DrunkRobotComponent():
  orcaice::Component("DrunkRobot"), 
  mainLoop_(NULL),
  geom_(new Position2dGeometry)
{

}

DrunkRobotComponent::~DrunkRobotComponent(){

}

void
DrunkRobotComponent::start(){

  // Get a reasonable geometry. 

  geom_->offset.p.x = 0; 
  geom_->offset.p.y = 0; 
  geom_->offset.o = 0; 


  geom_->size.l = 0.1; 
  geom_->size.w = 0.1;  


  IceStorm::TopicPrx topicPrx = orcaice::connectToTopicWithTag<Position2dConsumerPrx>
    ( context(), position2dConsumer_, "Position2d" );

  //First set up our proxies so other people can talk to us:
  position2dObj_ = new Position2dI( posBuffer_, geom_, topicPrx);
  orcaice::createInterfaceWithTag( context(), position2dObj_, "Position2d" );
  
  // Start the component. 

  activate();

  // then we get the main loop started. 
  mainLoop_ = new DrunkRobotMainLoop( posBuffer_, position2dConsumer_);

  mainLoop_->start();

}

void
DrunkRobotComponent::stop()
{
  cout<<"stopping loop"<<endl;
  // Tell the main loop to stop and wait for it to comply
  orcaice::Thread::stopAndJoin( mainLoop_ );
}
