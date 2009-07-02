/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
//! logs the lot (i.e. the entire history) on command.
class SnapshotLogger {
public:
    virtual ~SnapshotLogger() {}

    // format is any string.  timeWindowSec is the length of history
    // that will be logged in a snapshot.
    virtual void init( const std::string &format, double timeWindowSec )=0;
    virtual void subscribe( const std::string &interfaceTag )=0;

    // Call these before and after taking the snapshot
    virtual void prepareForSnapshot( const LogWriterInfo &logWriterInfo,
                                     MasterFileWriter &masterFileWriter ) = 0;
    // This is always safe to call, even if prepareForSnapshot() hasn't been called.
    virtual void finaliseSnapshot()=0;

    // Call these during snapshotting
    virtual unsigned int snapshotBufferSize() const=0;
    virtual const orca::Time &oldestArrivalTime() const=0;
    virtual void writeOldestObjToLogAndDiscard()=0;
};

}

#endif
