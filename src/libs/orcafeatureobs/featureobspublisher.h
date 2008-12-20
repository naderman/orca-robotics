#ifndef ORCAFEATUREOBS_FEATUREOBSPUBLISHER_H
#define ORCAFEATUREOBS_FEATUREOBSPUBLISHER_H

#include <hydrofeatureobs/ifeatureobspublisher.h>
#include <orcaifaceimpl/polarfeature2d.h>

namespace orcafeatureobs {

//!
//! @brief Publishes range scans out an orca interface
//!
//! @author Alex Brooks
//!
class FeatureObsPublisher : public hydrofeatureobs::IFeatureObsPublisher
{

public: 

    FeatureObsPublisher( const orca::PolarFeature2dDescription &descr,
                         const std::string                     &interfaceTag,
                         const orcaice::Context                &context )
        : polarfeature2dImpl_(new orcaifaceimpl::PolarFeature2dImpl(descr,interfaceTag,context))
        {}

    void initInterface()
        { polarfeature2dImpl_->initInterface(); }

    // From hydrofeatureobs::IFeatureObsPublisher
    void publish( const std::vector<hydrofeatureobs::FeatureObs*> &features );

private: 

    orcaifaceimpl::PolarFeature2dImplPtr polarfeature2dImpl_;

};

}

#endif
