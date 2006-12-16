/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_MISC_UTILITIES_H
#define ORCAOBJ_MISC_UTILITIES_H

#include <orca/featuremap2d.h>
#include <orca/localise2d.h>
#include <orca/ogmap.h>
#include <orca/pathfollower2d.h>

#include <stdio.h>  // for FILE on QNX

namespace orcaice
{
/*!
 *  @name Miscillaneous Object Manipulation Functions
 */
//@{

//! Normalise the Mixture of Gaussian that Localise2dDataPtr represents,
//! to ensure that the distribution integrates to one (ie the weights sum to 1)
//! It is an error to give this function a MoG with all zero weights.
void normalise( orca::Localise2dDataPtr &obj );

//!
//! Returns the max-likelihood hypothesis
//!
orca::Pose2dHypothesis &mlHypothesis( const orca::Localise2dDataPtr &obj );

//!
//! If there's something wrong with the pathData objects, returns false
//! and fills out the 'reason' field.
//! If it's OK, returns true.
//!
bool isSane( const orca::PathFollower2dDataPtr &pathData, std::string &reason );

//!
//! Display OG map cell
//!
char displayOgmapCell(unsigned char cell);

//! Get a reference to a grid cell (no bounds checking is performed)
inline unsigned char &gridCell( const orca::OgMapDataPtr &map, int indX, int indY )
{ return map->data[indY*map->numCellsX + indX]; }

//! Throws std::string on exceptions
void saveToFile(   const orca::FeatureMap2dDataPtr &fmap, FILE *f );

//! Throws std::string on exceptions
void loadFromFile( const std::string &filename, orca::FeatureMap2dDataPtr &fmap );

//@}

} // namespace

#endif
