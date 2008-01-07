/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_MISC_UTILITIES_H
#define ORCAOBJ_MISC_UTILITIES_H

#include <orca/featuremap2d.h>
#include <orca/localise2d.h>
#include <orca/localise3d.h>
#include <orca/ogmap.h>
#include <orca/pathfollower2d.h>

#include <stdio.h>  // for FILE on QNX

namespace orcaice
{
/*!
 *  @name Miscellaneous Object Manipulation Functions
 */
//@{

//! Normalise the Mixture of Gaussian that Localise2dDataPtr represents,
//! to ensure that the distribution integrates to one (ie the weights sum to 1)
//! It is an error to give this function a MoG with all zero weights.
void normalise( orca::Localise2dData& obj );

//! Calculates the angle increment for a range scan.
//! This is in a special function because it's easy to forget the '+1'
inline double calcAngleIncrement( double fieldOfView, int numReturns )
{ return fieldOfView / double(numReturns-1); }

//! Returns the @b reference to max-likelihood hypothesis.
const orca::Pose2dHypothesis& mlHypothesis( const orca::Localise2dData& obj );
const orca::Pose3dHypothesis& mlHypothesis( const orca::Localise3dData& obj );
bool localisationIsUncertain( const orca::Localise2dData &localiseData,
                              double linearThreshold = 5 );

//!
//! If there's something wrong with the pathData objects, returns false
//! and fills out the 'reason' field.
//! If it's OK, returns true.
//!
bool isSane( const orca::PathFollower2dData& pathData, std::string& reason );

//!
//! Display OG map cell
//!
char displayOgmapCell(unsigned char cell);

//! Get a @b reference to a grid cell (no bounds checking is performed)
inline unsigned char& gridCell( orca::OgMapData& obj, int indX, int indY )
{ 
    return obj.data[ indY*obj.numCellsX + indX ]; 
}

//! Calculate index of a cell in a grid map based on its row and column.
inline int gridCellIndex( const orca::OgMapData& obj, int indX, int indY )
{ 
    return indY*obj.numCellsX + indX; 
}

//! Throws hydroutil::Exception on problems
void saveToFile( const orca::FeatureMap2dData &fmap, FILE *f );

//! Throws hydroutil::Exception on problems
void loadFromFile( const std::string &filename, orca::FeatureMap2dData &fmap );

//@}

} // namespace

#endif
