/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_LOCALISE3D_H
#define ORCAOBJ_LOCALISE3D_H

#include <orca/localise3d.h>

namespace orcaobj
{

//! Returns the @b reference to max-likelihood hypothesis.
const orca::Pose3dHypothesis& mlHypothesis( const orca::Localise3dData& obj );

//! Converts to string
std::string toString( const orca::Localise3dData& );

} // namespace

#endif
