#include "autologgers.h"

namespace orcalogfactory {

LaserScanner2dAutoLogger::LaserScanner2dAutoLogger( const orcaice::Context &context ) :
        orcaifaceimpl::ConsumerImpl<orca::RangeScanner2d, orca::RangeScanner2dConsumer,
                          orca::RangeScanner2dDataPtr>( context )
{
}

void 
LaserScanner2dAutoLogger::dataEvent(const orca::RangeScanner2dDataPtr& data )
{ 
    // special case: do not copy to other loggers!
    // we assume that the data is really LaserScanner2dDataPtr but it has to be cast
    // @todo: what if it's not the right type?
    orca::LaserScanner2dDataPtr laserData = orca::LaserScanner2dDataPtr::dynamicCast( data );
    logWriter_->write(laserData,orcaice::getNow());
}

void 
LaserScanner2dAutoLogger::init( const orcalog::LogWriterInfo &logWriterInfo, 
                    orcalog::MasterFileWriter    &masterFileWriter )
{
    logWriter_.reset( new LaserScanner2dLogWriter );
    logWriter_->checkFormat( logWriterInfo.format );
    logWriter_->init( logWriterInfo, masterFileWriter );
}

void 
LaserScanner2dAutoLogger::startLogging()
{
    orca::LaserScanner2dPrx providerPrx;
    orcaice::connectToInterfaceWithTag( context_,
                                        providerPrx,
                                        logWriter_->logWriterInfo().interfaceTag );
    orca::RangeScanner2dDescription descr = providerPrx->getDescription();

    logWriter_->write( descr );

    subscribeWithTag( logWriter_->logWriterInfo().interfaceTag );
}

//////////////////////////////////////////////////////////////////////

CameraAutoLogger::CameraAutoLogger( const orcaice::Context &context ) :
        orcaifaceimpl::ConsumerImpl<orca::Image, orca::ImageConsumer, orca::ImageDataPtr>( context )
{
}

void 
CameraAutoLogger::dataEvent( const orca::ImageDataPtr& data )
{ 
    // we assume that the data is really CameraDataPtr but it has to be cast
    orca::ImageDataPtr cameraData = orca::ImageDataPtr::dynamicCast( data );
    logWriter_->write(cameraData,orcaice::getNow());
}

void 
CameraAutoLogger::init( const orcalog::LogWriterInfo &logWriterInfo, 
                    orcalog::MasterFileWriter    &masterFileWriter )
{
    logWriter_.reset( new CameraLogWriter );
    logWriter_->checkFormat( logWriterInfo.format );
    logWriter_->init( logWriterInfo, masterFileWriter );
}

void 
CameraAutoLogger::startLogging()
{
    orca::CameraPrx providerPrx;
    orcaice::connectToInterfaceWithTag( context_,
                                        providerPrx,
                                        logWriter_->logWriterInfo().interfaceTag );
    orca::CameraDescriptionPtr descr = orca::CameraDescriptionPtr::dynamicCast( providerPrx->getDescription());;

    logWriter_->write( descr );

    subscribeWithTag( logWriter_->logWriterInfo().interfaceTag );
}

}
