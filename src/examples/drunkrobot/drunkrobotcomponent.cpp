#include <iostream>


// Include headers for the main loop and components. 

#include "drunkrobotcomponent.h"
#include "drunkrobotmainloop.h"

//Also include headers for position2d_i.h

#include "position2d_i.h"

using namespace std;
using namespace orca;
using namespace orcaiceutil; 

DrunkRobotComponent::DrunkRobotComponent():
  orcaiceutil::Component("DrunkRobot"), 
  mainLoop_(NULL),
  geom_(new Position2dGeometry)
{

}

DrunkRobotComponent::~DrunkRobotComponent(){

}

void DrunkRobotComponent::start(){

  // Get a reasonable geometry. 

  geom_->offset.p.x = 0; 
  geom_->offset.p.y = 0; 
  geom_->offset.o = 0; 


  geom_->size.l = 0.1; 
  geom_->size.w = 0.1;  


  //First set up our proxies so other people can talk to us:
  position2dObj_ = new Position2dI( posBuffer_, geom_ );
  adapter()->add( position2dObj_, orcaiceutil::getProvidedNameAsIdentity(current(),"Position2d") );

  // Find IceStorm ConsumerProxy to push out data
  orcaiceutil::connectToIceStormConsumer<Position2dConsumerPrx>
          ( current(), position2dConsumer_, "Position2d" );

  //
  // ENABLE NETWORK CONNECTIONS
  //
  activate();

  // the we get the main loop started. 
  mainLoop_ = new DrunkRobotMainLoop( posBuffer_, position2dConsumer_);

  mainLoop_->start();

}


void DrunkRobotComponent::stop()
{
  cout<<"stopping loop"<<endl;
  // Tell the main loop to stop

  mainLoop_->stop();

  cout<<"joining thread"<<endl;
  // Then wait for it
  mainLoop_->getThreadControl().join();
}
