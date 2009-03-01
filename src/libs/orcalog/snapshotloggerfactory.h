/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_LOGGER_SNAPSHOT_LOGGER_FACTORY_H
#define ORCA_LOGGER_SNAPSHOT_LOGGER_FACTORY_H

#include <orcalog/factory.h>
#include <orcalog/snapshotlogger.h>
#include <orcalog/logwriterinfo.h>

namespace orcalog
{

//! Factory of SnapshotLogger instances.
class SnapshotLoggerFactory : public Factory
{
public:
    virtual ~SnapshotLoggerFactory() {};

    //! Creates a SnapshotLogger. Returns a pointer to it if
    //! successful or a NULL pointer if it fails.
    virtual SnapshotLogger* create( const std::string &interfaceType )=0;
};

} // namespace


// Function for instantiating plug-in factories.
// A plug-in library must have a function like so:
// extern "C" {
//     orcalog::SnapshotLoggerFactory *createSnapshotLoggerFactory();
// }
typedef orcalog::SnapshotLoggerFactory *SnapshotLoggerFactoryMakerFunc();


#endif
