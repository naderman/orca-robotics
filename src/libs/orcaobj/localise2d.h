/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_LOCALISE2D_H
#define ORCAOBJ_LOCALISE2D_H

#include <orca/localise2d.h>

namespace orcaobj
{

//! Normalise the Mixture of Gaussian that Localise2dDataPtr represents,
//! to ensure that the distribution integrates to one (ie the weights sum to 1)
//! It is an error to give this function a MoG with all zero weights.
void normalise( orca::Localise2dData& obj );

//! Returns the @b reference to max-likelihood hypothesis.
const orca::Pose2dHypothesis& mlHypothesis( const orca::Localise2dData& obj );

bool localisationIsUncertain( const orca::Localise2dData &localiseData,
                              double linearThreshold = 5 );

//! Converts to string
std::string toString( const orca::Localise2dData& );

} // namespace

#endif
