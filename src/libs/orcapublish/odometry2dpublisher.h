#ifndef ORCAPUBLISH_ODOMETRY2DPUBLISHER_H
#define ORCAPUBLISH_ODOMRTRY2DPUBLISHER_H

#include <hydropublish/odometry2dpublisher.h>
#include <orcanavutil/orcanavutil.h>
#include <orcaifaceimpl/odometry2d.h>

namespace orcapublish {

//!
//! @brief Publishes odom information on an orca interface
//!
//! @author Alex Brooks
//!
class Odometry2dPublisher : public hydropublish::Odometry2dPublisher
{
public:

    Odometry2dPublisher( const orca::VehicleDescription &vehicleDescr,
                         const orcaice::Context         &context,
                         const std::string              &interfaceString )
        : odometry2dImpl_(new orcaifaceimpl::Odometry2dImpl(vehicleDescr,context,interfaceString))
        {}

    void init()
        { odometry2dImpl_->initInterface(); }

    // From hydropublish::Odometry2dPublisher
    void localSetAndSend( const hydronavutil::Pose     &pose,
                          const hydronavutil::Velocity &velocity,
                          const hydrotime::Time        &time,
                          bool odometryWasReset=false )
        {
            orca::Odometry2dData orcaData;

            orcaData.timeStamp.seconds  = time.seconds();
            orcaData.timeStamp.useconds = time.useconds();

            orcaData.pose       = orcanavutil::convert( pose );
            orcaData.motion.v.x = velocity.lin();
            orcaData.motion.v.y = 0.0;
            orcaData.motion.w   = velocity.rot();

            orcaData.odometryWasReset = odometryWasReset;

            odometry2dImpl_->localSetAndSend( orcaData );
        }

private:

    orcaifaceimpl::Odometry2dImplPtr odometry2dImpl_;

};

//!
//! @brief Knows how to create Odometry2dPublisher's
//!
//! @author Alex Brooks
//!
class Odometry2dPublisherFactory : public hydropublish::Odometry2dPublisherFactory
{
public:

    Odometry2dPublisherFactory( const orcaice::Context &context )
        : context_(context)
        {}

    hydropublish::Odometry2dPublisherPtr createPublisher( 
        const hydronavutil::DiffDriveControlDescription    &controlDescription,
        const hydronavutil::CylindricalGeometryDescription &cylindricalDescription,
        const std::string                                  &name )
        {
            return hydropublish::Odometry2dPublisherPtr(
                new orcapublish::Odometry2dPublisher( orcanavutil::convert( controlDescription,
                                                               cylindricalDescription ),
                                         context_,
                                         name ) ); 
        }

private:

    orcaice::Context context_;
};

}

#endif
