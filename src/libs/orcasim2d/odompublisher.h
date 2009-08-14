#ifndef ORCASIM2D_ODOMPUBLISHER_H
#define ORCASIM2D_ODOMPUBLISHER_H

#include <hydrosim2d/iodompublisher.h>
#include <orcanavutil/orcanavutil.h>
#include <orcaifaceimpl/odometry2d.h>

namespace orcasim2d {

//!
//! @brief Publishes odom information on an orca interface
//!
//! @author Alex Brooks
//!
class OdomPublisher : public hydrosim2d::IOdomPublisher
{

public: 

    OdomPublisher( const orca::VehicleDescription &vehicleDescr,
                   const std::string              &interfaceTag,
                   const orcaice::Context         &context )
        : odometry2dImpl_(new orcaifaceimpl::Odometry2dImpl(vehicleDescr,interfaceTag,context))
        {}

    OdomPublisher( const orca::VehicleDescription &vehicleDescr,
                   const orcaice::Context         &context,
                   const std::string              &interfaceString )
        : odometry2dImpl_(new orcaifaceimpl::Odometry2dImpl(vehicleDescr,context,interfaceString))
        {}

    void initInterface()
        { odometry2dImpl_->initInterface(); }

    // From hydrosim2d::IOdomPublisher
    void publish( const hydronavutil::Pose     &pose,
                  const hydronavutil::Velocity &velocity );

private: 

    orcaifaceimpl::Odometry2dImplPtr odometry2dImpl_;

};

}

#endif
