#ifndef BRUCE2_OGFUSION_I_H
#define BRUCE2_OGFUSION_I_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/ogfusion.h>

// for context()
#include <orcaice/orcaice.h>

// utilities
#include <orcaice/ptrbuffer.h>

namespace ognode
{

class OgFusionI : public orca::OgFusion
{
public:
    OgFusionI(orca::OgFusionConfigPtr &config, orcaice::PtrBuffer<orca::OgFusionDataPtr> &OgFusionDataBuffer);
    virtual void setData(const orca::OgFusionDataPtr&, const Ice::Current&);
    virtual orca::OgFusionConfigPtr getConfig(const ::Ice::Current& ) const;
private:
    //config
    orca::OgFusionConfigPtr config_;

    //buffer
    orcaice::PtrBuffer<orca::OgFusionDataPtr> &ogFusionDataBuffer_;


};

} // namespace

#endif
