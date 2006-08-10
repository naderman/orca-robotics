#ifndef _BRUCE_OGNODE_COMPONENT_H_
#define _BRUCE_OGNODE_COMPONENT_H_

#include <orca/ogfusion.h>
#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>
#include <orcaogmap/orcaogmap.h>

namespace ognode
{

class Handler;

class Component : public orcaice::Component
{
public:
    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:
    Handler *handler_;

    orcaice::PtrBuffer<orca::OgFusionDataPtr> OgFusionDataBuffer_;
    Ice::ObjectPtr ogFusionObjPtr_;
    Ice::ObjectPtr ogMapObjPtr_;
    orcaogmap::OgMap localMap_;
};

} // namespace

#endif
