/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
// #include <iostream>
#include <orcaice/orcaice.h>

#include "localiseutil.h"

using namespace std;
using namespace orcaice;

namespace orcalocalnav {

bool localisationIsUncertain( const orca::Localise2dData &localiseData )
{
    // Some dodgy heuristics
    if ( localiseData.hypotheses.size() > 2 )
        return true;

    const orca::Pose2dHypothesis h = orcaice::mlHypothesis( localiseData );
    if ( h.cov.xx > 5 ||
         h.cov.yy > 5 )
        return true;

    return false;
}


} // end of namespace
