#ifndef ORCALOGFACTORY_AUTOLOGGERS_H
#define ORCALOGFACTORY_AUTOLOGGERS_H

#include <orcalog/autologger.h>
#include <orcalogfactory/logwriters.h>
#include <orcaice/connectutils.h>
#include <orcaice/icestormutils.h>
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
    template<class DataType,
             class ConsumerType,
             class ConsumerPrxType,
             class PrxType,
             class LogWriterType>
    class GenericAutoLogger : public ConsumerType, public orcalog::AutoLogger
    {
    public:

        virtual ~GenericAutoLogger() {}

        // Assumes the ConsumerType's method is called setData
        virtual void setData(const DataType& data, const Ice::Current&)
            { logWriter_->write(data,orcaice::getNow()); }

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
                PrxType objectPrx;
                orcaice::connectToInterfaceWithTag<PrxType>( logWriter_->logWriterInfo().context,
                                                             objectPrx,
                                                             logWriter_->logWriterInfo().interfaceTag );

                // Allow derived classes to do something special (like get description)
                setup(objectPrx,*logWriter_);
    
                Ice::ObjectPtr consumer = this;
                ConsumerPrxType callbackPrx = 
                    orcaice::createConsumerInterface<ConsumerPrxType>( logWriter_->logWriterInfo().context,
                                                                       consumer );

                objectPrx->subscribe( callbackPrx );
            }

    protected:

        // Called during initialisation
        virtual void setup( PrxType &objectPrx, LogWriterType &logWriter ) {}

    private:

        std::auto_ptr<LogWriterType> logWriter_;
    };

    //////////////////////////////////////////////////////////////////////

    typedef GenericAutoLogger<orca::CpuData,
                              orca::CpuConsumer,
                              orca::CpuConsumerPrx,
                              orca::CpuPrx,
                              CpuLogWriter> CpuAutoLogger;

    //////////////////////////////////////////////////////////////////////

    class DriveBicycleAutoLogger : public GenericAutoLogger<orca::DriveBicycleData,
                                                            orca::DriveBicycleConsumer,
                                                            orca::DriveBicycleConsumerPrx,
                                                            orca::DriveBicyclePrx,
                                                            DriveBicycleLogWriter>
    {
    private:
        void setup( orca::DriveBicyclePrx &objectPrx, DriveBicycleLogWriter &logWriter )
            {
                logWriter.write( objectPrx->getDescription() );
            }
    };

    //////////////////////////////////////////////////////////////////////

    typedef GenericAutoLogger<orca::ImuData,
                              orca::ImuConsumer,
                              orca::ImuConsumerPrx,
                              orca::ImuPrx,
                              ImuLogWriter> ImuAutoLogger;

    //////////////////////////////////////////////////////////////////////

    class LaserScanner2dAutoLogger : public orca::RangeScanner2dConsumer, public orcalog::AutoLogger
    {
    public:
        virtual ~LaserScanner2dAutoLogger() {}

        virtual void setData(const orca::RangeScanner2dDataPtr& data, const Ice::Current&)
            { 
                // special case: do not copy to other loggers!
                // we assume that the data is really LaserScanner2dDataPtr but it has to be cast
                // @todo: what if it's not the right type?
                orca::LaserScanner2dDataPtr laserData = orca::LaserScanner2dDataPtr::dynamicCast( data );
                logWriter_->write(laserData,orcaice::getNow());
            }

        virtual void init( const orcalog::LogWriterInfo &logWriterInfo, 
                           orcalog::MasterFileWriter    &masterFileWriter )
            {
                logWriter_.reset( new LaserScanner2dLogWriter );
                logWriter_->checkFormat( logWriterInfo.format );
                logWriter_->init( logWriterInfo, masterFileWriter );
            }

        virtual void startLogging()
            {
                orca::LaserScanner2dPrx objectPrx;
                orcaice::connectToInterfaceWithTag( logWriter_->logWriterInfo().context,
                                                    objectPrx,
                                                    logWriter_->logWriterInfo().interfaceTag );

                orca::RangeScanner2dDescription descr = objectPrx->getDescription();
                logWriter_->write( descr );
    
                Ice::ObjectPtr consumer = this;
                orca::RangeScanner2dConsumerPrx callbackPrx = 
                    orcaice::createConsumerInterface<orca::RangeScanner2dConsumerPrx>( logWriter_->logWriterInfo().context,
                                                                                       consumer );

                objectPrx->subscribe( callbackPrx );
            }
    private:
        std::auto_ptr<LaserScanner2dLogWriter> logWriter_;
    };

    //////////////////////////////////////////////////////////////////////

    class Localise2dAutoLogger : public GenericAutoLogger<orca::Localise2dData,
                                                          orca::Localise2dConsumer,
                                                          orca::Localise2dConsumerPrx,
                                                          orca::Localise2dPrx,
                                                          Localise2dLogWriter>
    {
    private:
        void setup( orca::Localise2dPrx &objectPrx, Localise2dLogWriter &logWriter )
            {
                logWriter.write( objectPrx->getDescription() );
            }
    };

    //////////////////////////////////////////////////////////////////////

    class Localise3dAutoLogger : public GenericAutoLogger<orca::Localise3dData,
                                                            orca::Localise3dConsumer,
                                                            orca::Localise3dConsumerPrx,
                                                            orca::Localise3dPrx,
                                                            Localise3dLogWriter>
    {
    private:
        void setup( orca::Localise3dPrx &objectPrx, Localise3dLogWriter &logWriter )
            {
                logWriter.write( objectPrx->getDescription() );
            }
    };

    //////////////////////////////////////////////////////////////////////

    class Odometry2dAutoLogger : public GenericAutoLogger<orca::Odometry2dData,
                                                            orca::Odometry2dConsumer,
                                                            orca::Odometry2dConsumerPrx,
                                                            orca::Odometry2dPrx,
                                                            Odometry2dLogWriter>
    {
    private:
        void setup( orca::Odometry2dPrx &objectPrx, Odometry2dLogWriter &logWriter )
            {
                logWriter.write( objectPrx->getDescription() );
            }
    };

    //////////////////////////////////////////////////////////////////////

    class Odometry3dAutoLogger : public GenericAutoLogger<orca::Odometry3dData,
                                                            orca::Odometry3dConsumer,
                                                            orca::Odometry3dConsumerPrx,
                                                            orca::Odometry3dPrx,
                                                            Odometry3dLogWriter>
    {
    private:
        void setup( orca::Odometry3dPrx &objectPrx, Odometry3dLogWriter &logWriter )
            {
                logWriter.write( objectPrx->getDescription() );
            }
    };

    //////////////////////////////////////////////////////////////////////

    typedef GenericAutoLogger<orca::PolarFeature2dData,
                              orca::PolarFeature2dConsumer,
                              orca::PolarFeature2dConsumerPrx,
                              orca::PolarFeature2dPrx,
                              PolarFeature2dLogWriter> PolarFeature2dAutoLogger;

    //////////////////////////////////////////////////////////////////////
    
    typedef GenericAutoLogger<orca::PowerData,
                              orca::PowerConsumer,
                              orca::PowerConsumerPrx,
                              orca::PowerPrx,
                              PowerLogWriter> PowerAutoLogger;

    //////////////////////////////////////////////////////////////////////

    typedef GenericAutoLogger<orca::WifiData,
                              orca::WifiConsumer,
                              orca::WifiConsumerPrx,
                              orca::WifiPrx,
                              WifiLogWriter> WifiAutoLogger;

    //////////////////////////////////////////////////////////////////////

    class GpsAutoLogger : public GenericAutoLogger<orca::GpsData,
                                                            orca::GpsConsumer,
                                                            orca::GpsConsumerPrx,
                                                            orca::GpsPrx,
                                                            GpsLogWriter>
    {
    private:
        void setup( orca::GpsPrx &objectPrx, GpsLogWriter &logWriter )
            {
                logWriter.write( objectPrx->getDescription() );
            }
    };

    //////////////////////////////////////////////////////////////////////

    class ImageAutoLogger : public GenericAutoLogger<orca::ImageDataPtr,
                                                            orca::ImageConsumer,
                                                            orca::ImageConsumerPrx,
                                                            orca::ImagePrx,
                                                            ImageLogWriter>
    {
    private:
        void setup( orca::ImagePrx &objectPrx, ImageLogWriter &logWriter )
            {
                logWriter.write( objectPrx->getDescription() );
            }
    };

    //////////////////////////////////////////////////////////////////////

    class CameraAutoLogger : public orca::ImageConsumer, public orcalog::AutoLogger
    {
    public:
        virtual ~CameraAutoLogger() {}

        virtual void setData(const orca::ImageDataPtr& data, const Ice::Current&)
            { 
                // we assume that the data is really CameraDataPtr but it has to be cast
                // @todo: what if it's not the right type?
                orca::CameraDataPtr cameraData = orca::CameraDataPtr::dynamicCast( data );
                logWriter_->write(cameraData,orcaice::getNow());
            }

        virtual void init( const orcalog::LogWriterInfo &logWriterInfo, 
                           orcalog::MasterFileWriter    &masterFileWriter )
            {
                logWriter_.reset( new CameraLogWriter );
                logWriter_->checkFormat( logWriterInfo.format );
                logWriter_->init( logWriterInfo, masterFileWriter );
            }

        virtual void startLogging()
            {
                orca::CameraPrx objectPrx;
                orcaice::connectToInterfaceWithTag( logWriter_->logWriterInfo().context,
                                                    objectPrx,
                                                    logWriter_->logWriterInfo().interfaceTag );

              //can't get the camera description?
//                orca::CameraDescriptionPtr descr = objectPrx->getDescription();
                orca::ImageDescriptionPtr descr = objectPrx->getDescription();
                logWriter_->write( descr );
    
                Ice::ObjectPtr consumer = this;
                orca::ImageConsumerPrx callbackPrx = 
                    orcaice::createConsumerInterface<orca::ImageConsumerPrx>( logWriter_->logWriterInfo().context,consumer );

                objectPrx->subscribe( callbackPrx );
            }
    private:
        std::auto_ptr<CameraLogWriter> logWriter_;
    };

}

#endif
