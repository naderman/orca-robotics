#include "featureobspublisher.h"
#include <iostream>
#include <orcaice/orcaice.h>
//#include "conversions.h"

using namespace std;

namespace orcafeatureobs {

namespace {

orca::SinglePolarFeature2dPtr convert( const hydrofeatureobs::FeatureObs &feature )
{
    orca::SinglePolarFeature2dPtr orcaF;

    {
        const hydrofeatureobs::PointFeatureObs *f = 
            dynamic_cast<const hydrofeatureobs::PointFeatureObs*>(&feature);
        if ( f != NULL )
        {
            orca::PointPolarFeature2dPtr pf = new orca::PointPolarFeature2d;
            orcaF = pf;
            pf->p.r       = f->range();
            pf->p.o       = f->bearing();
            pf->rangeSd   = f->rangeSd();
            pf->bearingSd = f->bearingSd();
        }
    }
    {
        const hydrofeatureobs::PoseFeatureObs *f = 
            dynamic_cast<const hydrofeatureobs::PoseFeatureObs*>(&feature);
        if ( f != NULL )
        {
            orca::PosePolarFeature2dPtr pf = new orca::PosePolarFeature2d;
            orcaF             = pf;
            pf->p.r           = f->range();
            pf->p.o           = f->bearing();
            pf->orientation   = f->orientation();
            pf->rangeSd       = f->rangeSd();
            pf->bearingSd     = f->bearingSd();
            pf->orientationSd = f->orientationSd();
        }
    }
    {
        const hydrofeatureobs::LineFeatureObs *f = 
            dynamic_cast<const hydrofeatureobs::LineFeatureObs*>(&feature);
        if ( f != NULL )
        {
            orca::LinePolarFeature2dPtr lf = new orca::LinePolarFeature2d;
            orcaF = lf;
            lf->start.r      = f->rangeStart();
            lf->start.o      = f->bearingStart();
            lf->end.r        = f->rangeEnd();
            lf->end.o        = f->bearingEnd();
            lf->rhoSd        = f->rhoSd();
            lf->alphaSd      = f->alphaSd();
            lf->startSighted = f->startSighted();
            lf->endSighted   = f->endSighted();
        }
    }
    assert( orcaF != 0 );

    orcaF->type           = feature.featureType();
    orcaF->pFalsePositive = feature.pFalsePositive();
    orcaF->pTruePositive  = feature.pTruePositive();
    return orcaF;
}    

}

void
FeatureObsPublisher::publish( const std::vector<hydrofeatureobs::FeatureObs*> &features )
{
    orca::PolarFeature2dData orcaData;
    for ( size_t i=0; i < features.size(); i++ )
    {
        orca::SinglePolarFeature2dPtr f( convert( *features[i] ) );
        orcaData.features.push_back( f );
        //orcaData.features.push_back( convert( *features[i] ) );
    }
    orcaData.timeStamp = orcaice::getNow();

    polarfeature2dImpl_->localSetAndSend( orcaData );
}

}

