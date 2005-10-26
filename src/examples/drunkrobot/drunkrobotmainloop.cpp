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
	position2dConsumer_->consumeData( currentPos );
      }
    catch ( const Ice::CommunicatorDestroyedException & e )
      {
	cout<<"The comm man is dead"<<endl;
	// it's ok, the communicator may already be destroyed
      }
    
    // Then we put it in the buffer for pull requests. 
    posBuffer_.push( currentPos );

    // And now, sleep for a little while
    sleep(1);
    
  }

}
