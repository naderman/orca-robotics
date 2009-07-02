/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_FEATUREMAP2D_INTERFACE_ICE
#define ORCA2_FEATUREMAP2D_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/localise2d.ice>
#include <IceStorm/IceStorm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_featuremap2d FeatureMap2d
    @brief A map of features, eg for SLAM purposes

    @{
*/

module feature
{
    //! A laser-reflective beacon.
    const int LASERREFLECTOR   = 0;
    //! Something like a pole or a tree.
    const int FOREGROUNDPOINT  = 1;
    //! An opening in a wall, represented by the mid-point of the opening.
    const int DOOR             = 2;
    //! The junction of two lines.
    const int CORNER           = 3;
    //! A line segment.
    const int LINE             = 4;
    //! corner on eg the inside of a room
    const int INTERNALCORNER   = 5;
    //! corner on eg the outside of a building
    const int EXTERNALCORNER   = 6;
    //! Pose of a vehicle
    const int VEHICLEPOSE      = 7;
    //! A GPS Fix
    const int GPSFIX           = 8;
};

//! Generic 2D feature.
class Feature2d
{
    //! Feature type.
    int type;

    //! The probability that the feature is really there
    double pExists;
};

//! 2d Covariance Matrix
struct PointCovariance2d {
    //! Matrix index (0,0)
    double xx;
    //! Matrix index (0,1)
    double xy;
    //! Matrix index (1,1)
    double yy;
};

//! A basic x-y point feature
class CartesianPointFeature2d extends Feature2d
{
    //! mean
    CartesianPoint2d  p;
    //! covariance descrbing Gaussian uncertainty
    PointCovariance2d c;
};

//!
//! A line feature.
//!
//! Infinite lines are parameterised by (rho,alpha), which describes
//! the vector from the origin which meets the line at 90deg:
//!   - rho   is the length of this vector, and
//!   - alpha is the angle the vector makes with the x-axis (in (-pi,pi]).
//!
//! This information is augmented with info that describes the fact that
//! the line is a finite segment, visible from one direction only.
//!
//! By definition, a line is visible iff the start appears to the
//! right of the end.
//!
class CartesianLineFeature2d extends Feature2d
{
    //! mean rho
    double rho;
    //! mean alpha
    double alpha;
    //! covariance of (rho,alpha)
    PointCovariance2d c;

    //! start of line
    CartesianPoint2d  start;
    //! end of line
    CartesianPoint2d  end;

    //! Is the reported start point the actual extent of the line? 
    bool startSighted;

    //! Is the reported end point the actual extent of the line? 
    bool endSighted;
};

//! A point feature with a heading
class CartesianPoseFeature2d extends Feature2d
{
    //! mean
    Frame2d      p;
    //! uncertainty
    Covariance2d c;
};

//! A list of features
sequence<Feature2d> Feature2dSequence;

//! Data describing generic 2D features.
struct FeatureMap2dData
{
    //! Features
    Feature2dSequence features;
};

//! Consumer of FeatureMap2d data
interface FeatureMap2dConsumer
{    
    //! Transmits the data to the consumer.
    void setData( FeatureMap2dData obj );
};

//! An interface to a map of features.
interface FeatureMap2d
{
    //! Returns the latest data.
    idempotent FeatureMap2dData getData();

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( FeatureMap2dConsumer* subscriber )
        throws SubscriptionFailedException, SubscriptionPushFailedException;
};

//!  //@}
}; // module

#endif
