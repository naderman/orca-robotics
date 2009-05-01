/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCASYSTEMSTATUSUTIL_UTILS_H
#define ORCASYSTEMSTATUSUTIL_UTILS_H

#include <orca/systemstatus.h>

namespace orcasystemstatusutil {

//! Returns TRUE if status if component health is not OK or if the component is stalled.
bool isStatusAlert( const orca::ComponentStatusEpisode& episode );

//! Returns maximum number of components among the platforms of the system.
int maxPlatformSize( const orca::SystemStatusData& );

}

#endif
