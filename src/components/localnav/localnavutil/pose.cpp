/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "pose.h"
#include <iostream>
#include <sstream>

// for mlHypothesis()
#include <orcaobj/miscutils.h>

using namespace std;

namespace localnav {

hydronavutil::Pose
getMLPose( const orca::Localise2dData &localiseData )
{
    const orca::Pose2dHypothesis &h = orcaobj::mlHypothesis( localiseData );
    return hydronavutil::Pose( h.mean.p.x, h.mean.p.y, h.mean.o );
}

}
