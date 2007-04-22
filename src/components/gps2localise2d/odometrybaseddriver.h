/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ODOMETRY_BASED_DRIVER_H
#define ODOMETRY_BASED_DRIVER_H

#include "driver.h"
#include <orcaice/context.h>
#include "simpledriver.h"
#include <orcaifaceimpl/consumertypes.h>
#include <orcanavutil/offset.h>
#include <orcanavutil/odometrydifferentiator.h>

namespace gps2localise2d {

//!
//! @brief Assumes that the GPS unit works out heading by differentiating.
//!
//!        Works out heading uncertainty  based on this assumption, plus the odometric velocity
//!        of the platform.
//!
//! @author Alex Brooks
//!
class OdometryBasedDriver : public Driver
{

public: 

    OdometryBasedDriver( const orca::GpsDescription &descr, const orcaice::Context &context );
    ~OdometryBasedDriver();

    // Converts the GPS info into localise2d format.
    // Returns 'false' if the conversion can't be done (eg because GPS doesn't have a fix).
    bool compute( const orca::GpsData &gpsData, orca::Localise2dData &localiseData );

private: 

    bool setup();
    double calcHeadingUncertainty( orcanavutil::Offset &delta,
                                   double dt );

    SimpleDriver                                simpleDriver_;
    orcaifaceimpl::ProxiedOdometry2dConsumerPtr odomConsumer_;
    bool                                        isSetup_;

    orcanavutil::OdometryDifferentiator         odometryDifferentiator_;

    orca::Time prevTime_;

    double minHeadingUncertainty_;
    double linSpeedThreshold_;
    double rotFactor_;
    double linFactor_;

    orcaice::Context                            context_;

};

}

#endif
