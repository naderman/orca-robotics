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

namespace {

bool isStatusNovel( const orca::SubsystemStatus& prev, 
                    const orca::SubsystemStatus& curr )
{
    return ( curr.state!=prev.state ||
             curr.health!=prev.health ||
             curr.message!=prev.message ||
             curr.isStalled!=prev.isStalled );
}

bool isStatusNovel( const orca::SubsystemStatusDict& prev, 
                    const orca::SubsystemStatusDict& curr )
{
    if ( prev.size() != curr.size() )
        return true;

    orca::SubsystemStatusDict::const_iterator previt;
    orca::SubsystemStatusDict::const_iterator currit = curr.begin();

    for ( previt=prev.begin(); previt!=prev.end(); ++previt ) {
        if ( previt->first!=currit->first || isStatusNovel( previt->second, currit->second ) )
            return true;
        ++currit;
    }

    return false;
}

}

///////////////////////////////////

bool isStatusAlert( const orca::ComponentStatusEpisode& episode )
{
    return ( episode.health != orca::CompOk || episode.isStalled );
};

bool isStatusNovel( const orca::ComponentStatusEpisode& prev, 
                    const orca::ComponentStatusEpisode& curr )
{
    return ( curr.state!=prev.state ||
             curr.health!=prev.health ||
             curr.isStalled!=prev.isStalled ||
             isStatusNovel( prev.subsystems, curr.subsystems ) );
}

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
