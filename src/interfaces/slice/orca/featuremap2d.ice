/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_FEATUREMAP2D_INTERFACE_ICE
#define ORCA2_FEATUREMAP2D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>
#include <orca/localise2d.ice>

module orca
{
/*!
    @ingroup interfaces
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
    //! An opening in a wall.
    const int DOOR             = 2;
    //! The junction of two lines.
    const int CORNER           = 3;
//     //! The end of a wall.  The junction of two walls looks 
//     //! like this when viewed from a position such that the 
//     //! other wall is obscured.  The other wall may not exist however.
//     const int POSSIBLECORNER   = 4;
    //! A line segment.
    const int LINE             = 4;
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

//! A list of features
sequence<Feature2d> Feature2dSequence;

//! Data describing generic 2D features.
class FeatureMap2dData extends OrcaObject
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
    nonmutating FeatureMap2dData getData();
    
    /*!
     * Mimics IceStorm's subscribe(). @p subscriber is typically a direct proxy to the consumer object.
     * The implementation may choose to implement the push directly or use IceStorm.
     * This choice is transparent to the subscriber. The case when the @p subscriber is already subscribed
     * is quietly ignored.
     *
     * @see unsubscribe
     */
    void subscribe( FeatureMap2dConsumer *subscriber );

    /*!
     * Unsubscribe an existing @p subscriber. The case when the @p subscriber is not subscribed
     * is quietly ignored.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( FeatureMap2dConsumer *subscriber );
};

//!  //@}
}; // module

#endif
