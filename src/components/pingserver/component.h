#ifndef PINGSERVER_COMPONENT_H
#define PINGSERVER_COMPONENT_H

#include <orcaice/component.h>
#include <orca/tracer.h>
#include "pingerI.h"

namespace pingserver {

//!
//! @author Alex Brooks
//!
class Component : public orcaice::Component
{

public: 

    Component();

    // component interface
    virtual void start();
    virtual void stop();

private: 

    PingerI *pingerInterface_;
};

}

#endif
