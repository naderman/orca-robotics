#ifndef ORCA2_DR_MAIN_LOOP_H
#define ORCA2_DR_MAIN_LOOP_H


#include <orcaiceutil/thread.h>
#include <orcaiceutil/ptrbuffer.h>

#include <orca/position2d.h>

//! Implements the main loop of our drunken robot. 
//! It's very convenient to inherit from orcaiceutil::Thread
//! as it means really we only have to implement one method: 
//! run(). 


class DrunkRobotMainLoop: public orcaiceutil::Thread {
 public: 

  // The posData pointer tells us where to put the data
  // The Consumer is a proxy for IceStorm, we will use that to pass data to 
  // Ice storm. 
  DrunkRobotMainLoop(orcaiceutil::PtrBuffer<orca::Position2dDataPtr> &posData,
		     const orca::Position2dConsumerPrx &position2dConsumer);

  ~DrunkRobotMainLoop(); 

  virtual void run();

 private: 
  orcaiceutil::PtrBuffer<orca::Position2dDataPtr>    & posBuffer_;

  orca::Position2dConsumerPrx position2dConsumer_;
};

#endif
