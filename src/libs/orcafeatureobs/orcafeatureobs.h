#ifndef ORCAFEATUREOBS_CONVERSIONS_H
#define ORCAFEATUREOBS_CONVERSIONS_H

#include <hydrofeatureobs/hydrofeatureobs.h>
#include <orca/polarfeature2d.h>

namespace orcafeatureobs {

// conversion between Orca and Hydro datatypes
orca::SinglePolarFeature2dPtr convert( const hydrofeatureobs::FeatureObs &hydroFeature );

}

#endif
