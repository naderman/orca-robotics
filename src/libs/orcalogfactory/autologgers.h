#ifndef ORCALOGFACTORY_AUTOLOGGERS_H
#define ORCALOGFACTORY_AUTOLOGGERS_H

#include <orcaifaceimpl/consumerImpl.h>
#include <orcalog/autologger.h>
#include <orcalogfactory/logwriters.h>
#include <orcaice/timeutils.h>

namespace orcalogfactory {

//
// A set of AutoLoggers: ie loggers which run continuously as soon as they're
//                       initialised
//

//////////////////////////////////////////////////////////////////////

//!
//! The generic version which will fit most use-cases.
//! has a hook to allow something special to happen on initialisation (eg 'getDescription')
//!
template<class ProviderType, 
         class ConsumerType,  
         class ObjectType,
         class LogWriterType>
class GenericAutoLogger : 
        public orcaifaceimpl::ConsumerImpl<ProviderType,ConsumerType,ObjectType>, 
        public orcalog::AutoLogger
{
    using orcaifaceimpl::ConsumerSubscriber::context_;
    typedef typename ProviderType::ProxyType ProviderPrxType;
    typedef typename ConsumerType::ProxyType ConsumerPrxType;

public:
    GenericAutoLogger( const orcaice::Context &context ) :
        orcaifaceimpl::ConsumerImpl<ProviderType,ConsumerType,ObjectType>( context )
    {
    }

    virtual void dataEvent( const ObjectType& data ) 
    {
        logWriter_->write( data, orcaice::getNow() );
    }

    // Inherited from orcalog::AutoLogger
    virtual void init( const orcalog::LogWriterInfo &logWriterInfo,
                        orcalog::MasterFileWriter    &masterFileWriter )
    {
        logWriter_.reset( new LogWriterType );
        logWriter_->checkFormat( logWriterInfo.format );
        logWriter_->init( logWriterInfo, masterFileWriter );
    }

    virtual void startLogging()
    {
        // may throw
        ProviderPrxType providerPrx;
        orcaice::connectToInterfaceWithTag<ProviderPrxType>( 
                context_, 
                providerPrx,
                logWriter_->logWriterInfo().interfaceTag );

        // Allow derived classes to do something special (like get description)
        setupEvent( providerPrx, *logWriter_ );

        subscribeWithTag( logWriter_->logWriterInfo().interfaceTag );
    }

protected:

    // Customizable event, called during initialisation
    virtual void setupEvent( ProviderPrxType &providerPrx, LogWriterType &logWriter ) {}

private:

    std::auto_ptr<LogWriterType> logWriter_;
};


//////////////////////////////////////////////////////////////////////

typedef GenericAutoLogger<orca::Cpu, orca::CpuConsumer,
                          orca::CpuData,
                          CpuLogWriter> CpuAutoLogger;

//////////////////////////////////////////////////////////////////////

class DriveBicycleAutoLogger : public GenericAutoLogger<orca::DriveBicycle, orca::DriveBicycleConsumer,
                                                        orca::DriveBicycleData,
                                                        DriveBicycleLogWriter>
{
public:
    DriveBicycleAutoLogger( const orcaice::Context &context ) :
        GenericAutoLogger<orca::DriveBicycle, orca::DriveBicycleConsumer,
                          orca::DriveBicycleData,
                          DriveBicycleLogWriter>( context ) {};
private:
    void setupEvent( orca::DriveBicyclePrx &providerPrx, DriveBicycleLogWriter &logWriter )
        {
            logWriter.write( providerPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

typedef GenericAutoLogger<orca::Imu, orca::ImuConsumer,
                          orca::ImuData,
                          ImuLogWriter> ImuAutoLogger;

//////////////////////////////////////////////////////////////////////

class Localise2dAutoLogger : public GenericAutoLogger<orca::Localise2d, orca::Localise2dConsumer,
                                                      orca::Localise2dData,
                                                      Localise2dLogWriter>
{
public:
    Localise2dAutoLogger( const orcaice::Context &context ) :
        GenericAutoLogger<orca::Localise2d, orca::Localise2dConsumer,
                          orca::Localise2dData,
                          Localise2dLogWriter>( context ) {};
private:
    void setupEvent( orca::Localise2dPrx &providerPrx, Localise2dLogWriter &logWriter )
        {
            logWriter.write( providerPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

class Localise3dAutoLogger : public GenericAutoLogger<orca::Localise3d, orca::Localise3dConsumer,
                                                      orca::Localise3dData,
                                                      Localise3dLogWriter>
{
public:
    Localise3dAutoLogger( const orcaice::Context &context ) :
        GenericAutoLogger<orca::Localise3d, orca::Localise3dConsumer,
                          orca::Localise3dData,
                          Localise3dLogWriter>( context ) {};
private:
    void setupEvent( orca::Localise3dPrx &providerPrx, Localise3dLogWriter &logWriter )
        {
            logWriter.write( providerPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

class Odometry2dAutoLogger : public GenericAutoLogger<orca::Odometry2d, orca::Odometry2dConsumer,
                                                      orca::Odometry2dData,
                                                      Odometry2dLogWriter>
{
public:
    Odometry2dAutoLogger( const orcaice::Context &context ) :
        GenericAutoLogger<orca::Odometry2d, orca::Odometry2dConsumer,
                          orca::Odometry2dData,
                          Odometry2dLogWriter>( context ) {};
private:
    void setupEvent( orca::Odometry2dPrx &providerPrx, Odometry2dLogWriter &logWriter )
        {
            logWriter.write( providerPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

class Odometry3dAutoLogger : public GenericAutoLogger<orca::Odometry3d, orca::Odometry3dConsumer,
                                                      orca::Odometry3dData,
                                                      Odometry3dLogWriter>
{
public:
    Odometry3dAutoLogger( const orcaice::Context &context ) :
        GenericAutoLogger<orca::Odometry3d, orca::Odometry3dConsumer,
                        orca::Odometry3dData,
                        Odometry3dLogWriter>( context ) {};
private:
    void setupEvent( orca::Odometry3dPrx &providerPrx, Odometry3dLogWriter &logWriter )
        {
            logWriter.write( providerPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

typedef GenericAutoLogger<orca::PolarFeature2d, orca::PolarFeature2dConsumer,
                          orca::PolarFeature2dData,
                          PolarFeature2dLogWriter> PolarFeature2dAutoLogger;

//////////////////////////////////////////////////////////////////////

typedef GenericAutoLogger<orca::Power, orca::PowerConsumer,
                          orca::PowerData,
                          PowerLogWriter> PowerAutoLogger;

//////////////////////////////////////////////////////////////////////

typedef GenericAutoLogger<orca::Wifi, orca::WifiConsumer,
                          orca::WifiData,
                          WifiLogWriter> WifiAutoLogger;

//////////////////////////////////////////////////////////////////////

class GpsAutoLogger : public GenericAutoLogger<orca::Gps, orca::GpsConsumer,
                                              orca::GpsData,
                                              GpsLogWriter>
{
public:
    GpsAutoLogger( const orcaice::Context &context ) :
        GenericAutoLogger<orca::Gps, orca::GpsConsumer,
                        orca::GpsData,
                        GpsLogWriter>( context ) {};
private:
    void setupEvent( orca::GpsPrx &providerPrx, GpsLogWriter &logWriter )
        {
            logWriter.write( providerPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

class ImageAutoLogger : public GenericAutoLogger<orca::Image, orca::ImageConsumer,
                                                orca::ImageDataPtr,
                                                ImageLogWriter>
{
public:
    ImageAutoLogger( const orcaice::Context &context ) :
        GenericAutoLogger<orca::Image, orca::ImageConsumer,
                          orca::ImageDataPtr,
                          ImageLogWriter>( context ) {};
private:
    void setupEvent( orca::ImagePrx &providerPrx, ImageLogWriter &logWriter )
        {
            logWriter.write( providerPrx->getDescription() );
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

}

#endif
