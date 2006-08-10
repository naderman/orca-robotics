#include <iostream>

#include "ogfusionI.h"

using namespace std;
using namespace ognode;

OgFusionI::OgFusionI(orca::OgFusionConfigPtr &config,
		     orcaice::PtrBuffer<orca::OgFusionDataPtr> &OgFusionDataBuffer)
    :config_(config),
    ogFusionDataBuffer_(OgFusionDataBuffer)
{
}

void
OgFusionI::setData(const orca::OgFusionDataPtr& data, const Ice::Current& context)
{
    //cout << "got an object with " << data->observation.size() << " cells\n";
    ogFusionDataBuffer_.push(data);
}

orca::OgFusionConfigPtr
OgFusionI::getConfig(const ::Ice::Current& ) const
{
    return config_;
}
