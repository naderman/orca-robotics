/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_STOCHASTIC_H
#define ORCAOBJ_STOCHASTIC_H

#include <orca/stochastic.h>

namespace orcaobj
{

//! Sets all fields to specified values
void setInit( orca::Covariance2d&, double pxx=0.0, double pxy=0.0, double pyy=0.0, double pxt=0.0, double pyt=0.0, double ptt=0.0 );

//! Converts to string
std::string toString( const orca::Covariance2d& );


} // namespace

#endif
