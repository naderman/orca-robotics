/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_INTERFACES_RANGESCAN_NAV2D_H
#define HYDRO_INTERFACES_RANGESCAN_NAV2D_H

#include <hydroutil/context.h>
// #include <exception>

namespace hydrointerfaces 
{

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_rangescannav2d RangeScanNav2d drivers
    @brief Implementations of RangeScanNav2d algorithm interface.

    @b EXPERIMENTAL. No implementations yet!
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_rangescannav2d RangeScanNav2d
    @brief Local navigation using 2D range scan.

    @b EXPERIMENTAL. No implementations yet!
    @{
*/

/*!

@brief Abstract interface class for local navigation drivers.

The manager sets the goal location (in the robot's coordinate system)
by modifying the goal.

@author Alex Brooks

*/
class RangeScanNav2d
{

public: 

    //! Motion command structure
    class Command
    {
    public:
        //! Desired longitudinal velocity [m/s]
        double lin;
        //! Desired yaw rate [rad/s]
        double rot;
    
        //! Converts to a human readable string
        std::string toString() const;
    };

    virtual ~RangeScanNav2d() {};

    //! The number of waypoints we look into the future (and therefore
    //! expect to be provided with)
    virtual int waypointHorizon() { return 1; }

    //! Sets motion command based on the provided state and goal information.
    //!
    //! The pose is in a global coordinate frame, while the goals are
    //! in the local coordinate frame.
    virtual void getCommand( 
                bool                                   stalled,
                bool                                   localisationUncertain,
//                 const hydronavutil::Pose               &pose,
//                 const orca::Twist2d                   &currentVelocity,
//                 const orca::Time                      &poseAndVelocityTime,
//                 const orca::RangeScanner2dDataPtr      obs,
//                 const std::vector<orcalocalnav::Goal> &goals 
                   Command& command ) = 0;
};

//! Helper class to instantiate drivers
class RangeScanNav2dFactory {
public:
    virtual ~RangeScanNav2dFactory() {};
    //! Instantiates a driver and returns a pointer to it.
    virtual RangeScanNav2d *createDriver( const hydroutil::Context& context ) const=0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::RangeScanNav2dFactory *createRangeScanNav2dDriverFactory();
// }
typedef hydrointerfaces::RangeScanNav2dFactory *RangeScanNav2dDriverFactoryMakerFunc();

#endif
