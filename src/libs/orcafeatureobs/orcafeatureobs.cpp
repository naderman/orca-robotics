#include "orcafeatureobs.h"
#include <iostream>
#include <sstream>
#include <gbxutilacfr/exceptions.h>

using namespace std;

namespace orcafeatureobs {

orca::SinglePolarFeature2dPtr convert( const hydrofeatureobs::FeatureObs &hydroFeature )
{
        orca::SinglePolarFeature2dPtr orcaFeature;

        bool foundConversion = false;
        if ( !foundConversion )
        {
            const hydrofeatureobs::PointFeatureObs *f =
                dynamic_cast<const hydrofeatureobs::PointFeatureObs*>(&hydroFeature);
            if ( f != NULL )
            {
                orca::PointPolarFeature2d *orcaF = new orca::PointPolarFeature2d;
                orcaFeature = orcaF;
                orcaF->p.r       = f->range();
                orcaF->p.o       = f->bearing();
                orcaF->rangeSd   = f->rangeSd();
                orcaF->bearingSd = f->bearingSd();
                foundConversion = true;
            }
        }
        if ( !foundConversion )
        {
            const hydrofeatureobs::PoseFeatureObs *f =
                dynamic_cast<const hydrofeatureobs::PoseFeatureObs*>(&hydroFeature);
            if ( f != NULL )
            {
                orca::PosePolarFeature2d *orcaF = new orca::PosePolarFeature2d;
                orcaFeature = orcaF;
                orcaF->p.r           = f->range();
                orcaF->p.o           = f->bearing();
                orcaF->orientation   = f->orientation();
                orcaF->rangeSd       = f->rangeSd();
                orcaF->bearingSd     = f->bearingSd();
                orcaF->orientationSd = f->orientationSd();
                foundConversion = true;
            }
        }
        if ( !foundConversion )
        {
            const hydrofeatureobs::LineFeatureObs *f =
                dynamic_cast<const hydrofeatureobs::LineFeatureObs*>(&hydroFeature);
            if ( f != NULL )
            {
                orca::LinePolarFeature2d *orcaF = new orca::LinePolarFeature2d;
                orcaFeature = orcaF;
                orcaF->start.r      = f->rangeStart();
                orcaF->start.o      = f->bearingStart();
                orcaF->end.r        = f->rangeEnd();
                orcaF->end.o        = f->bearingEnd();
                orcaF->startSighted = f->startSighted();
                orcaF->endSighted   = f->endSighted();
                orcaF->rhoSd        = f->rhoSd();
                orcaF->alphaSd      = f->alphaSd();
                foundConversion = true;
            }
        }
        if ( !foundConversion )
        {
            stringstream ss;
            ss << "Unable to convert: " << hydroFeature.toString();
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }        
        orcaFeature->type           = hydroFeature.featureType();
        orcaFeature->pFalsePositive = hydroFeature.pFalsePositive();
        orcaFeature->pTruePositive  = hydroFeature.pTruePositive();

        return orcaFeature;    
}

}

