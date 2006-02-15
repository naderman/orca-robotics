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

module orca
{
/*!
    @ingroup interfaces
    @author Alex Brooks
    @defgroup orca_interface_featuremap2d FeatureMap2d
    @brief A map of features, eg for SLAM purposes

    @{
*/

module feature
{
    const int LASERREFLECTOR   = 0;
    const int FOREGROUNDPOINT  = 1;
    const int DOOR             = 2;
    const int CORNER           = 3;
    const int POSSIBLECORNER   = 4;
};

// Define messages first
class Feature2d
{
    int type;
};

class CartesianPointFeature2d extends Feature2d
{
    CartesianPoint2d p;
};

// class GaussCartesianPointFeature2d extends CartesianPointFeature
// {
//    ...    
// };

//! A list of features
sequence<Feature2d> CartesianFeature2dSequence;

//! Data -- the 2D polar features
class FeatureMap2dData extends OrcaObject
{
    CartesianFeature2dSequence features;
};

//! Consumer of FeatureMap2d data
interface FeatureMap2dConsumer
{    
    void setData( FeatureMap2dData obj );
};

//! An interface to a map of features.
interface FeatureMap2d
{
    nonmutating FeatureMap2dData getData();
    void subscribe( FeatureMap2dConsumer *subscriber );
    idempotent void unsubscribe( FeatureMap2dConsumer *subscriber );
};


}; // module

#endif
