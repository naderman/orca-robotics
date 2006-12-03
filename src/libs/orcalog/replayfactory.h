/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOG_REPLAY_FACTORY_H
#define ORCA2_ORCALOG_REPLAY_FACTORY_H

#include <orcalog/factory.h>

namespace orcaice
{
    class Context;
}

namespace orcalog
{

class Replayer;
    
//! Factory of log replayers.
class ReplayFactory : public Factory
{
public:
    virtual ~ReplayFactory() {};

    //! Creates a log replayer of specified type. Returns a pointer to it if
    //! successful or a NULL pointer if it fails.
    virtual Replayer* create( 
        const std::string      &interfaceType, 
        const std::string      &format,
        const std::string      &filename,
        const orcaice::Context &context) = 0;
};

} // namespace

// Function for instantiating plug-in factories.
// A plug-in library must have a function like so:
// extern "C" {
//     orcalog::ReplayerFactory *createReplayFactory();
// }
typedef orcalog::ReplayFactory *ReplayFactoryMakerFunc();

#endif
