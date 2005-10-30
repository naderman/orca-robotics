/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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

#ifndef ORCA2_POLARFEATURE_INTERFACE_ICE
#define ORCA2_POLARFEATURE_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @author Tobias Kaupp t.kaupp at cas.edu.au
    @defgroup orca_interface_polarfeature2d PolarFeature2D
    @brief Range/bearing features relative to robot frame

The polarfeature2d interface provides access to features in a polar coordinate system
centered on the robots centre. The data returned by the interface consists of a sequence of
features with range in meters and bearing in rad (-pi<angle<pi) and the feature type.
    @{
*/

// Define messages first
sequence<PolarPoint2d> PolarPoint2dSequence;
sequence<int> FeatureType;


//! Data -- the 2D polar features
class PolarFeature2dData extends OrcaObject
{
    PolarPoint2dSequence features;
    FeatureType featureTypes;
};

//! Consumer of PolarFeature2d data
interface PolarFeature2dConsumer
{    
    void setData( PolarFeature2dData obj );
};

//! PolarFeature2D Interface
interface PolarFeature2d
{
    nonmutating PolarFeature2dData getData();
    void subscribe( PolarFeature2dConsumer *subscriber );
    idempotent void unsubscribe( PolarFeature2dConsumer *subscriber );
};


}; // module

#endif
