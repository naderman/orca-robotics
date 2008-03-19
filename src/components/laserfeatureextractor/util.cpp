#include "util.h"
#include <iostream>

using namespace std;

namespace laserfeatures {

orca::SinglePolarFeature2dPtr convert( const hydrofeatureobs::FeatureObs &hydroFeature )
{
    orca::SinglePolarFeature2dPtr orcaFeature;

    if ( hydroFeature.isPointFeature() )
    {
        const hydrofeatureobs::PointFeatureObs *f =
            dynamic_cast<const hydrofeatureobs::PointFeatureObs*>(&hydroFeature);
        assert( f != NULL );

        orca::PointPolarFeature2d *orcaF = new orca::PointPolarFeature2d;
        orcaFeature = orcaF;
        orcaF->p.r       = f->range();
        orcaF->p.o       = f->bearing();
        orcaF->rangeSd   = f->rangeSd();
        orcaF->bearingSd = f->bearingSd();
    }
    else if ( hydroFeature.isLineFeature() )
    {
        const hydrofeatureobs::LineFeatureObs *f =
            dynamic_cast<const hydrofeatureobs::LineFeatureObs*>(&hydroFeature);
        assert( f != NULL );

        orca::LinePolarFeature2d *orcaF = new orca::LinePolarFeature2d;
        orcaFeature = orcaF;
        orcaF->start.r      = f->start().range();
        orcaF->start.o      = f->start().bearing();
        orcaF->end.r        = f->end().range();
        orcaF->end.o        = f->end().bearing();
        orcaF->startSighted = f->startSighted();
        orcaF->endSighted   = f->endSighted();
        orcaF->rhoSd        = f->rhoSd();
        orcaF->alphaSd      = f->alphaSd();
    }

    orcaFeature->type           = hydroFeature.featureType();
    orcaFeature->pFalsePositive = hydroFeature.pFalsePositive();
    orcaFeature->pTruePositive  = hydroFeature.pTruePositive();

    return orcaFeature;
}


}

