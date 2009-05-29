/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_FEATUREMAP2D_H
#define ORCAOBJ_FEATUREMAP2D_H

#include <orca/featuremap2d.h>

namespace orcaobj
{

//! Throws gbxutilacfr::Exception on problems
void saveToFile( const orca::FeatureMap2dData &fmap, FILE *f );

//! Throws gbxutilacfr::Exception on problems
void loadFromFile( const std::string &filename, orca::FeatureMap2dData &fmap );

//! Converts to string
std::string toString( const orca::FeatureMap2dData& );
//! Converts to string
std::string toString( const orca::Feature2d& );

} // namespace

#endif
