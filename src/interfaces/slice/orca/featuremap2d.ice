/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
    //! The junction of two walls.
    const int CORNER           = 3;
    //! The end of a wall.  The junction of two walls looks 
    //! like this when viewed from a position such that the 
    //! other wall is obscured.  The other wall may not exist however.
    const int POSSIBLECORNER   = 4;
};

//! Generic 2D feature.
class Feature2d
{
    //! Feature type.
    int type;
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

class CartesianPointFeature2d extends Feature2d
{
    CartesianPoint2d  p;
    PointCovariance2d c;
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
