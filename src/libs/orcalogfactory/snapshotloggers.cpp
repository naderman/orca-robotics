#include <iostream>
#include "snapshotloggers.h"

namespace orcalogfactory {

LaserScanner2dSnapshotLogger::LaserScanner2dSnapshotLogger( const orcaice::Context &context ) :
        orcaifaceimpl::ConsumerImpl<orca::RangeScanner2d,
                                    orca::RangeScanner2dConsumer,
                                    orca::RangeScanner2dDataPtr>( context ),
        isTakingSnapshot_(false)
{
}

void 
LaserScanner2dSnapshotLogger::dataEvent(const orca::RangeScanner2dDataPtr& data )
{ 
    {
        // Don't accept new data while taking a snapshot.
        IceUtil::Mutex::Lock lock(mutex_);
        if ( isTakingSnapshot_ ) return;
    }
    // special case: do not copy to other loggers!
    // we assume that the data is really LaserScanner2dDataPtr but it has to be cast
    // @todo: what if it's not the right type?
    orca::LaserScanner2dDataPtr laserData = orca::LaserScanner2dDataPtr::dynamicCast( data );
    snapshotLogBuffer_->addItem( laserData );
}

void 
LaserScanner2dSnapshotLogger::init( const std::string &format, double timeWindowSec )
{
    LaserScanner2dLogWriter dummyLogWriter;
    dummyLogWriter.checkFormat( format );
    snapshotLogBuffer_.reset( new orcalog::SnapshotLogBuffer<orca::LaserScanner2dDataPtr,
                                                              LaserScanner2dLogWriter>( timeWindowSec ) );
}

void 
LaserScanner2dSnapshotLogger::prepareForSnapshot( const orcalog::LogWriterInfo &logWriterInfo,
                                  orcalog::MasterFileWriter    &masterFileWriter )
{
    {
        IceUtil::Mutex::Lock lock(mutex_);
        isTakingSnapshot_ = true;
    }
    logWriter_.reset( new LaserScanner2dLogWriter );
    logWriter_->init( logWriterInfo, masterFileWriter );
    
    // Allow interface-specific stuff to happen (eg getting description)
    // doSpecialisedSnapshotPreparation( *logWriter_ );
    // may throw
    orca::LaserScanner2dPrx providerPrx;
    orcaice::connectToInterfaceWithTag( logWriter_->logWriterInfo().context,
                                        providerPrx,
                                        logWriter_->logWriterInfo().interfaceTag );
    logWriter_->write( providerPrx->getDescription() );
}

void 
LaserScanner2dSnapshotLogger::finaliseSnapshot()
{
    {
        IceUtil::Mutex::Lock lock(mutex_);
        isTakingSnapshot_ = false;
    }
}

}
