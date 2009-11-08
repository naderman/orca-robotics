/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcaice/icegridutils.h>
#include "types.h"

using namespace std;

namespace orcacm
{

// need to define this for sorting the vector of homes
bool operator<(const HomeHeader& l, const HomeHeader& r)
{
    const string leftName = orcaice::toString( orcaice::toComponentName( l.proxy ) );
    const string rightName = orcaice::toString( orcaice::toComponentName( r.proxy ) );

    return (leftName<rightName);
}

} // namespace
