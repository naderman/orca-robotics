#ifndef FEATUREMAPLOADER_COMPONENT_H
#define FEATUREMAPLOADER_COMPONENT_H

#include <orcaice/component.h>
#include <orca/featuremap2d.h>

namespace featuremaploader {

//
// Deriving from orcaice::Component means we have to implement start() and stop()
//
class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    virtual void start();
    virtual void stop();

private:

    // Returns zero on success
    int loadMap( const std::string &fileName, orca::FeatureMap2dDataPtr &theMap );

    //
    // EXTERNAL INTERFACE: featuremap2d
    //
    Ice::ObjectPtr featureMap2dObj_;
};

}

#endif
