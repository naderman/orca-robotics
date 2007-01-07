/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_DEFAULT_REPLAY_FACTORY_H
#define ORCA2_ORCALOGFACTORY_DEFAULT_REPLAY_FACTORY_H

#include <string>
#include <map>

#include <orcalog/replayfactory.h>

namespace orcalog
{
    class ReplayMaster;
    class Replayer;
}

namespace orcalogfactory
{
    
class DefaultReplayFactory : public orcalog::ReplayFactory
{
    
public:
    DefaultReplayFactory();
    
    virtual orcalog::Replayer* create( 
        const std::string      &interfaceType, 
        const std::string      &format,
        const std::string      &filename,
        const orcaice::Context &context);
};

} // namespace

extern "C"
{
    orcalog::ReplayFactory *createReplayFactory();
}

#endif
