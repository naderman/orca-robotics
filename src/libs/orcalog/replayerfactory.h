/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCALOG_REPLAYER_FACTORY_H
#define ORCA_ORCALOG_REPLAYER_FACTORY_H

#include <orcalog/factory.h>
#include <orcalog/logreaderinfo.h>

namespace orcaice
{
    class Context;
}

namespace orcalog
{

class Replayer;
    
//! Factory of log replayers.
class ReplayerFactory : public Factory
{
public:
    virtual ~ReplayerFactory() {};

    //! Creates a log replayer of specified type. Returns a pointer to it if
    //! successful or a NULL pointer if it fails.
    virtual Replayer* create( const LogReaderInfo &logReaderInfo )=0;
};

} // namespace

// Function for instantiating plug-in factories.
// A plug-in library must have a function like so:
// extern "C" {
//     orcalog::ReplayerFactory *createReplayerFactory();
// }
typedef orcalog::ReplayerFactory *ReplayerFactoryMakerFunc();

#endif
