/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_LOGGER_SNAPSHOT_LOGGER_H
#define ORCA_LOGGER_SNAPSHOT_LOGGER_H

#include <orcalog/logwriterinfo.h>
#include <orcalog/masterfilewriter.h>

namespace orcalog
{

//! An SnapshotLogger keeps a buffer of data it has received, and 
//! logs the lot on command.
class SnapshotLogger {
public:
    virtual ~SnapshotLogger() {}
    
    virtual void init( const std::string &format )=0;
    virtual void subscribe( orcaice::Context &context, const std::string &interfaceTag )=0;

    virtual void takeSnapshot( const LogWriterInfo &logWriterInfo,
                               MasterFileWriter &masterFileWriter ) = 0;
};

}

#endif
