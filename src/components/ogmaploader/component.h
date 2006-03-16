#ifndef MAPLOADER_COMPONENT_H
#define MAPLOADER_COMPONENT_H

#include <orcaice/component.h>
#include <orca/ogmap.h>

//!
//! Deriving from orcaice::Component means we have to implement start() and stop()
//!
class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    virtual void start();
    virtual void stop();

private:

    //
    // EXTERNAL INTERFACE
    //
    Ice::ObjectPtr ogMapObj_;
};

#endif
