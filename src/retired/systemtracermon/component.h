#ifndef COMPONENT_H
#define COMPONENT_H

#include <orcaice/component.h>
#include <orca/tracer.h>
#include "tracerconsumerI.h"

namespace sysmon {

//
// @author Alex Brooks
//
class Component : public orcaice::Component
{

public: 

    Component();

    // component interface
    virtual void start();
    virtual void stop();

private: 

    void connectForTracingInfo( orca::TracerPrx tracerPrx );

    TracerConsumerI *tracerConsumer_;
    orca::TracerConsumerPrx callbackPrx_;
};

}

#endif
