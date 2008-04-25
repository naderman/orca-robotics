#ifndef ORCASIM2D_POSEPUBLISHER_H
#define ORCASIM2D_POSEPUBLISHER_H

#include <hydrosim2d/iposepublisher.h>
#include <orcanavutil/orcanavutil.h>
#include <orcaifaceimpl/localise2dImpl.h>

namespace orcasim2d {

//!
//! @brief Publishes pose information on an orca::Localise2d interface
//!
//! @author Alex Brooks
//!
class PosePublisher : public hydrosim2d::IPosePublisher
{

public: 

    PosePublisher( const orca::VehicleGeometryDescriptionPtr &geomDescr,
                   const std::string                         &interfaceTag,
                   const orcaice::Context                    &context )
        : localise2dImpl_(new orcaifaceimpl::Localise2dImpl(geomDescr,interfaceTag,context))
        {}

    void initInterface()
        { localise2dImpl_->initInterface(); }

    // From hydrosim2d::IPosePublisher
    void publish( const hydronavutil::Pose &pose );

private: 

    orcaifaceimpl::Localise2dImplPtr localise2dImpl_;

};

}

#endif
