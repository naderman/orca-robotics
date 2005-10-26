
#ifndef ORCA2_DR_COMPONENT_H
#define ORCA2_DR_COMPONENT_H

#include <orcaiceutil/component.h> 
#include <orca/position2d.h>

#include <orcaiceutil/orcaiceutil.h>
#include <orcaiceutil/ptrnotify.h>


class DrunkRobotMainLoop; 

class DrunkRobotComponent : public orcaiceutil::Component
{
public:
    DrunkRobotComponent();
    ~DrunkRobotComponent();
    virtual void start();
    virtual void stop();

 private:

    DrunkRobotMainLoop *mainLoop_;

    // Now the buffers that go to the ICE interface:

    orcaiceutil::PtrBuffer<orca::Position2dDataPtr> posBuffer_; 

    orca::Position2dGeometryPtr geom_; 

    // This is used to set up our interface 

    Ice::ObjectPtr position2dObj_;

    // Finally, we also define a proxy to our IceStorm channel

    orca::Position2dConsumerPrx position2dConsumer_;

};

#endif
