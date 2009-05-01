/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "utils.h"

namespace orcasystemstatusutil {

bool isStatusAlert( const orca::ComponentStatusEpisode& episode )
{
    return ( episode.health != orca::CompOk || episode.isStalled );
};

int maxPlatformSize( const orca::SystemStatusData& data )
{
    unsigned int maxNum=0;
    
    for ( orca::SystemStatusDict::const_iterator it=data.systemStatus.begin(); it!=data.systemStatus.end(); ++it )
    {
        if ( it->second.size() > maxNum )
            maxNum = it->second.size();
    }
    
    return (int)maxNum;
}

}
