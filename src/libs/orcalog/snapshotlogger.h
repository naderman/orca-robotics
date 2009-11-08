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
#include <orcaifaceimpl/consumerImpl.h>
#include <IceUtil/Mutex.h>
#include <orcalog/snapshotlogbuffer.h>

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

//!
//! The generic version which will fit most use-cases.
//! has a hook to allow something special to happen on initialisation (eg 'getDescription')
//!
template<class ProviderType, 
         class ConsumerType,  
         class ObjectType,
         class LogWriterType>
    class GenericSnapshotLogger : 
                  public orcaifaceimpl::ConsumerImpl<ProviderType,ConsumerType,ObjectType>, 
                  public orcalog::SnapshotLogger
{
using orcaifaceimpl::ConsumerSubscriber::subscribeWithTag;

public:
    GenericSnapshotLogger( const orcaice::Context &context ) :
        orcaifaceimpl::ConsumerImpl<ProviderType,ConsumerType,ObjectType>( context ),
        isTakingSnapshot_(false)
    {
    }

    virtual void dataEvent( const ObjectType& data ) 
    {
        {
            // Don't accept new data while taking a snapshot.
            IceUtil::Mutex::Lock lock(mutex_);
            if ( isTakingSnapshot_ ) return;
        }
        snapshotLogBuffer_->addItem( data );
    }

    // Inherited from orcalog::SnapshotLogger
    virtual void subscribe( const std::string &interfaceTag )
        { subscribeWithTag( interfaceTag ); };

    virtual void init( const std::string &format, double timeWindowSec )
    {
        LogWriterType dummyLogWriter;
        dummyLogWriter.checkFormat( format );
        snapshotLogBuffer_.reset( new orcalog::SnapshotLogBuffer<ObjectType,
                                                                  LogWriterType>( timeWindowSec ) );
    }

    virtual void prepareForSnapshot( const orcalog::LogWriterInfo &logWriterInfo,
                                      orcalog::MasterFileWriter    &masterFileWriter )
    {
        {
            IceUtil::Mutex::Lock lock(mutex_);
            isTakingSnapshot_ = true;
        }
        logWriter_.reset( new LogWriterType );
        logWriter_->init( logWriterInfo, masterFileWriter );
        
        // Allow interface-specific stuff to happen (eg getting description)
        doSpecialisedSnapshotPreparation( *logWriter_ );
    }

    virtual void finaliseSnapshot()
    {
        {
            IceUtil::Mutex::Lock lock(mutex_);
            isTakingSnapshot_ = false;
        }
    }

    virtual unsigned int snapshotBufferSize() const
        { return snapshotLogBuffer_->bufferSize(); }
    virtual const orca::Time &oldestArrivalTime() const
        { return snapshotLogBuffer_->oldestArrivalTime(); }
    virtual void writeOldestObjToLogAndDiscard()
        { snapshotLogBuffer_->writeOldestObjToLogAndDiscard(*logWriter_); }

protected:

    // Called during initialisation
    virtual void doSpecialisedSnapshotPreparation( LogWriterType &logWriter ) {}

private:

    bool                                      isTakingSnapshot_;
    std::auto_ptr< orcalog::SnapshotLogBuffer<ObjectType,LogWriterType> > snapshotLogBuffer_;
    std::auto_ptr<LogWriterType>              logWriter_;
    IceUtil::Mutex                            mutex_;
};

}

#endif
