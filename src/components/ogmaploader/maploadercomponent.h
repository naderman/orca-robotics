#ifndef MAPLOADER_COMPONENT_H
#define MAPLOADER_COMPONENT_H

#include <orcaice/component.h>
#include <orca/ogmap.h>

//!
//! Deriving from orcaice::Component means we have to implement start() and stop()
//!
class MapLoaderComponent : public orcaice::Component
{
public:

    MapLoaderComponent();
    virtual ~MapLoaderComponent();

    virtual void start();
    virtual void stop();

private:

    //
    // EXTERNAL INTERFACE
    //
    Ice::ObjectPtr ogMapObj_;
};

#endif
