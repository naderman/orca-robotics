/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_LOGGER_DEFAULT_SNAPSHOT_LOGGER_FACTORY_H
#define ORCA_LOGGER_DEFAULT_SNAPSHOT_LOGGER_FACTORY_H

#include <orcalog/snapshotloggerfactory.h>
#include <orcalog/logwriterinfo.h>

namespace orcalogfactory
{

class DefaultSnapshotLoggerFactory: public orcalog::SnapshotLoggerFactory

{
public:
    DefaultSnapshotLoggerFactory();
    
    virtual orcalog::SnapshotLogger* create( const std::string &interfaceType, const orcaice::Context& context );
};

} // namespace

extern "C" {
    orcalog::SnapshotLoggerFactory *createSnapshotLoggerFactory();
}


#endif

