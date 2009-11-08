#ifndef ORCANAVUTIL_ORCANAVUTIL_H
#define ORCANAVUTIL_ORCANAVUTIL_H

#include <hydronavutil/hydronavutil.h>
#include <orca/localise2d.h>
#include <orca/odometry2d.h>
#include <orca/velocitycontrol2d.h>
#include <hydronavutil/vehicledescription.h>
#include <orca/vehicledescription.h>

namespace orcanavutil {

    //
    // Conversion Utilities
    //
    //////////////////////////////////////////////////////////////////////

    //
    // orca -> hydro
    //

    hydronavutil::Pose     convert( const orca::Frame2d &frame );
    hydronavutil::Cov3d    convert( const orca::Covariance2d &c );
    hydronavutil::Gaussian convert( const orca::Pose2dHypothesis &hyp );
    hydronavutil::Gmm      convert( const orca::Localise2dData &locData );
    hydronavutil::Pose     convertToPose( const orca::Odometry2dData &odom );
    hydronavutil::Velocity convertToVelocity( const orca::Odometry2dData &odom );
    //! throws away transverse component.
    hydronavutil::Velocity convert( const orca::VelocityControl2dData &cmd );
    //! throws away uncertainty information and all but the most likely component.
    hydronavutil::Pose     convertToPose( const orca::Localise2dData &loc );

    //
    // hydro -> orca
    //

    orca::Pose2dHypothesis convert( const hydronavutil::Gaussian &g, double weight );
    //! Note: the timestamp has to be supplied separately since the Gmm doesn't have one.
    orca::Localise2dData   convert( const hydronavutil::Gmm &gmm, int seconds, int useconds );
    //! Limits the number of hypotheses in the output.
    //! Setting maxHypotheses to less than zero converts all of them.
    //! Note: the timestamp has to be supplied separately since the Gmm doesn't have one.
    orca::Localise2dData convertUpToNHypotheses( const hydronavutil::Gmm &gmm,
                                                 int maxHypotheses,
                                                 int seconds,
                                                 int useconds );
    orca::Frame2d convert( const hydronavutil::Pose &pose );
    //! Sets transverse component to zero.
    orca::VelocityControl2dData convert( const hydronavutil::Velocity &cmd );
    //! Returns a covariance matrix of the form: diag([linearCov, linearCov, rotationalCov])
    orca::Covariance2d diagonalCovariance( double linearCov, double rotationalCov );

    orca::VehicleGeometryDescriptionPtr convert( const hydronavutil::CylindricalGeometryDescription &geomDescr );
    orca::VehicleDescription convert( const hydronavutil::DiffDriveControlDescription    &controlDescr,
                                      const hydronavutil::CylindricalGeometryDescription &geomDescr );
}

#endif
