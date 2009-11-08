#ifndef ORCALOGFACTORY_AUTOLOGGERS_H
#define ORCALOGFACTORY_AUTOLOGGERS_H

#include <orcalog/autologger.h>
#include <orcalogfactory/logwriters.h>
#include <orcaifaceimpl/pathfollower2d.h>

namespace orcalogfactory {

//
// A set of AutoLoggers: ie loggers which run continuously as soon as they're
//                       initialised
//

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericAutoLogger<orca::Cpu, orca::CpuConsumer,
                                   orca::CpuData,
                                   CpuLogWriter> CpuAutoLogger;

//////////////////////////////////////////////////////////////////////

class DriveBicycleAutoLogger : public orcalog::GenericAutoLogger<orca::DriveBicycle, orca::DriveBicycleConsumer,
                                                                 orca::DriveBicycleData,
                                                                 DriveBicycleLogWriter>
{
public:
    DriveBicycleAutoLogger( const orcaice::Context &context ) :
        orcalog::GenericAutoLogger<orca::DriveBicycle, orca::DriveBicycleConsumer,
                          orca::DriveBicycleData,
                          DriveBicycleLogWriter>( context ) {};
private:
    void setupEvent( orca::DriveBicyclePrx &providerPrx, DriveBicycleLogWriter &logWriter )
        {
            logWriter.write( providerPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericAutoLogger<orca::Imu, orca::ImuConsumer,
                          orca::ImuData,
                          ImuLogWriter> ImuAutoLogger;

//////////////////////////////////////////////////////////////////////

class Localise2dAutoLogger : public orcalog::GenericAutoLogger<orca::Localise2d, orca::Localise2dConsumer,
                                                               orca::Localise2dData,
                                                               Localise2dLogWriter>
{
public:
    Localise2dAutoLogger( const orcaice::Context &context ) :
        orcalog::GenericAutoLogger<orca::Localise2d, orca::Localise2dConsumer,
                          orca::Localise2dData,
                          Localise2dLogWriter>( context ) {};
private:
    void setupEvent( orca::Localise2dPrx &providerPrx, Localise2dLogWriter &logWriter )
        {
            logWriter.write( providerPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

class Localise3dAutoLogger : public orcalog::GenericAutoLogger<orca::Localise3d, orca::Localise3dConsumer,
                                                               orca::Localise3dData,
                                                               Localise3dLogWriter>
{
public:
    Localise3dAutoLogger( const orcaice::Context &context ) :
        orcalog::GenericAutoLogger<orca::Localise3d, orca::Localise3dConsumer,
                          orca::Localise3dData,
                          Localise3dLogWriter>( context ) {};
private:
    void setupEvent( orca::Localise3dPrx &providerPrx, Localise3dLogWriter &logWriter )
        {
            logWriter.write( providerPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

class Odometry2dAutoLogger : public orcalog::GenericAutoLogger<orca::Odometry2d, orca::Odometry2dConsumer,
                                                               orca::Odometry2dData,
                                                               Odometry2dLogWriter>
{
public:
    Odometry2dAutoLogger( const orcaice::Context &context ) :
        orcalog::GenericAutoLogger<orca::Odometry2d, orca::Odometry2dConsumer,
                          orca::Odometry2dData,
                          Odometry2dLogWriter>( context ) {};
private:
    void setupEvent( orca::Odometry2dPrx &providerPrx, Odometry2dLogWriter &logWriter )
        {
            logWriter.write( providerPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

class Odometry3dAutoLogger : public orcalog::GenericAutoLogger<orca::Odometry3d, orca::Odometry3dConsumer,
                                                               orca::Odometry3dData,
                                                               Odometry3dLogWriter>
{
public:
    Odometry3dAutoLogger( const orcaice::Context &context ) :
        orcalog::GenericAutoLogger<orca::Odometry3d, orca::Odometry3dConsumer,
                        orca::Odometry3dData,
                        Odometry3dLogWriter>( context ) {};
private:
    void setupEvent( orca::Odometry3dPrx &providerPrx, Odometry3dLogWriter &logWriter )
        {
            logWriter.write( providerPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericAutoLogger<orca::PolarFeature2d, orca::PolarFeature2dConsumer,
                                   orca::PolarFeature2dData,
                                   PolarFeature2dLogWriter> PolarFeature2dAutoLogger;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericAutoLogger<orca::Power, orca::PowerConsumer,
                          orca::PowerData,
                          PowerLogWriter> PowerAutoLogger;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericAutoLogger<orca::Wifi, orca::WifiConsumer,
                                   orca::WifiData,
                                   WifiLogWriter> WifiAutoLogger;

//////////////////////////////////////////////////////////////////////

class GpsAutoLogger : public orcalog::GenericAutoLogger<orca::Gps, orca::GpsConsumer,
                                              orca::GpsData,
                                              GpsLogWriter>
{
public:
    GpsAutoLogger( const orcaice::Context &context ) :
        orcalog::GenericAutoLogger<orca::Gps, orca::GpsConsumer,
                                   orca::GpsData,
                                   GpsLogWriter>( context ) {};
private:
    void setupEvent( orca::GpsPrx &providerPrx, GpsLogWriter &logWriter )
        {
            logWriter.write( providerPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

class ImageAutoLogger : public orcalog::GenericAutoLogger<orca::Image, orca::ImageConsumer,
                                                orca::ImageDataPtr,
                                                ImageLogWriter>
{
public:
    ImageAutoLogger( const orcaice::Context &context ) :
        orcalog::GenericAutoLogger<orca::Image, orca::ImageConsumer,
                                   orca::ImageDataPtr,
                                   ImageLogWriter>( context ) {};
private:
    void setupEvent( orca::ImagePrx &providerPrx, ImageLogWriter &logWriter )
        {
            logWriter.write( providerPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

class  MultiCameraAutoLogger : public orcalog::GenericAutoLogger<orca::MultiCamera,
                                                            orca:: MultiCameraConsumer,
                                                            orca::MultiCameraDataPtr,
                                                            MultiCameraLogWriter>
{
public:

    MultiCameraAutoLogger(const orcaice::Context &context) :
            orcalog::GenericAutoLogger<orca::MultiCamera, orca::MultiCameraConsumer,
                        orca::MultiCameraDataPtr,
                        MultiCameraLogWriter>(context) {};
    private:
        void setupEvent(orca::MultiCameraPrx &providerPrx, MultiCameraLogWriter &logWriter)
        {
            // MultiCameraWriter needs to know properties of the data in order to set up properly
            logWriter.initMultiCameraWriter(providerPrx->getDescription());
            logWriter.write(providerPrx->getDescription());
        }
};


//////////////////////////////////////////////////////////////////////
// the following do not use the generic template because their data
// types are classes and not structs
//////////////////////////////////////////////////////////////////////

class LaserScanner2dAutoLogger :
      public orcaifaceimpl::ConsumerImpl<orca::RangeScanner2d, orca::RangeScanner2dConsumer,
                          orca::RangeScanner2dDataPtr>, 
      public orcalog::AutoLogger
{
public:
    LaserScanner2dAutoLogger( const orcaice::Context &context );

    virtual void dataEvent( const orca::RangeScanner2dDataPtr& data );

    virtual void init( const orcalog::LogWriterInfo &logWriterInfo, 
                        orcalog::MasterFileWriter    &masterFileWriter );
    virtual void startLogging();

private:
    std::auto_ptr<LaserScanner2dLogWriter> logWriter_;
};

class RangeScanner2dAutoLogger :
      public orcaifaceimpl::ConsumerImpl<orca::RangeScanner2d, orca::RangeScanner2dConsumer,
                          orca::RangeScanner2dDataPtr>, 
      public orcalog::AutoLogger
{
public:
    RangeScanner2dAutoLogger( const orcaice::Context &context );

    virtual void dataEvent( const orca::RangeScanner2dDataPtr& data );

    virtual void init( const orcalog::LogWriterInfo &logWriterInfo, 
                        orcalog::MasterFileWriter    &masterFileWriter );
    virtual void startLogging();

private:
    std::auto_ptr<RangeScanner2dLogWriter> logWriter_;
};

//////////////////////////////////////////////////////////////////////

class CameraAutoLogger : 
      public orcaifaceimpl::ConsumerImpl<orca::Image, orca::ImageConsumer, orca::ImageDataPtr>,
      public orcalog::AutoLogger
{
public:    
    CameraAutoLogger( const orcaice::Context &context );

    virtual void dataEvent( const orca::ImageDataPtr& data );
    virtual void init( const orcalog::LogWriterInfo &logWriterInfo, 
                        orcalog::MasterFileWriter    &masterFileWriter );
    virtual void startLogging();

private:
    std::auto_ptr<CameraLogWriter> logWriter_;
};

//////////////////////////////////////////////////////////////////////

typedef orcaifaceimpl::ConsumerImpl<orca::PathFollower2d,
                                    orca::PathFollower2dConsumer,
                                    orca::PathFollower2dData,
                                    orcaifaceimpl::PathFollower2dConsumerTypeI> PathFollower2dConsumerImpl;

class PathFollower2dAutoLogger : public orcalog::AutoLogger,
                                 public PathFollower2dConsumerImpl
{
public:
    PathFollower2dAutoLogger( const orcaice::Context &context )
        : PathFollower2dConsumerImpl( context )
        {}

    void dataEvent( const orca::PathFollower2dData &data )
        {
            logWriter_->write( data, orcaice::getNow() );
        }

    //////////////////////////////////////////////////////////////////////
    // BEGIN cut-and-paste-and-modify from GenericAutoLogger
    //////////////////////////////////////////////////////////////////////
    // Inherited from orcalog::AutoLogger
    virtual void init( const orcalog::LogWriterInfo &logWriterInfo,
                       orcalog::MasterFileWriter    &masterFileWriter )
    {
        logWriter_.reset( new PathFollower2dLogWriter );
        logWriter_->checkFormat( logWriterInfo.format );
        logWriter_->init( logWriterInfo, masterFileWriter );
    }

    virtual void startLogging()
    {
        // may throw
        //orca::PathFollower2dPrx providerPrx;
        //orcaice::connectToInterfaceWithTag<orca::PathFollower2dPrx>( 
        //        context_, 
        //        providerPrx,
        //        logWriter_->logWriterInfo().interfaceTag );

        // Allow derived classes to do something special (like get description)
        // setupEvent( providerPrx, *logWriter_ );

        subscribeWithTag( logWriter_->logWriterInfo().interfaceTag );
    }
    //////////////////////////////////////////////////////////////////////
    // END cut-and-paste-and-modify from GenericAutoLogger
    //////////////////////////////////////////////////////////////////////
private:

    std::auto_ptr<PathFollower2dLogWriter> logWriter_;
};

}

#endif
