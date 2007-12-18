/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCALOGFACTORY_DEFAULT_REPLAYER_FACTORY_H
#define ORCA_ORCALOGFACTORY_DEFAULT_REPLAYER_FACTORY_H

#include <string>
#include <map>

#include <orcalog/replayerfactory.h>

namespace orcalog
{
    class ReplayerMaster;
    class Replayer;
}

namespace orcalogfactory
{
    
class DefaultReplayerFactory : public orcalog::ReplayerFactory
{
    
public:
    DefaultReplayerFactory();
    
    orcalog::Replayer* create( const orcalog::LogReaderInfo &logReaderInfo );
};

} // namespace

extern "C"
{
    orcalog::ReplayerFactory *createReplayerFactory();
}

#endif
