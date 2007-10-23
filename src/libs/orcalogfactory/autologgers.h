#ifndef ORCALOGFACTORY_AUTOLOGGERS_H
#define ORCALOGFACTORY_AUTOLOGGERS_H

#include <orcalog/autologgerfactory.h>
#include <orcalogfactory/logwriters.h>
#include <orcaice/connectutils.h>

namespace orcalogfactory {

    //////////////////////////////////////////////////////////////////////

    template<class DataType,
             class ConsumerType,
             class ConsumerPrxType,
             class PrxType,
             class LogWriterType>
    class GenericAutoLogger : public ConsumerType, public orcalog::AutoLogger
    {
    public:

        GenericAutoLogger( const orcalog::LogWriterInfo &logWriterInfo )
            : logWriter_(logWriterInfo)
            {}
        virtual ~GenericAutoLogger() {}

        virtual void setData(const DataType& data, const Ice::Current&)
            { logWriter_.write(data); }

        virtual void initAndStartLogging()
            {
                // may throw
                PrxType objectPrx;
                orcaice::connectToInterfaceWithTag<PrxType>( logWriter_.logWriterInfo().context,
                                                             objectPrx,
                                                             logWriter_.logWriterInfo().interfaceTag );

                // Allow derived classes to do something special (like get description)
                init(objectPrx);
    
                Ice::ObjectPtr consumer = this;
                ConsumerPrxType callbackPrx = 
                    orcaice::createConsumerInterface<ConsumerPrxType>( logWriter_.logWriterInfo().context,
                                                                       consumer );

                objectPrx->subscribe( callbackPrx );
            }

    protected:

        virtual void init( PrxType &objectPrx ) {}

        LogWriterType    logWriter_;
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
    public:
        DriveBicycleAutoLogger( const orcalog::LogWriterInfo &logWriterInfo )
            : GenericAutoLogger<orca::DriveBicycleData,
                                orca::DriveBicycleConsumer,
                                orca::DriveBicycleConsumerPrx,
                                orca::DriveBicyclePrx,
                                DriveBicycleLogWriter>( logWriterInfo )
            {}

        void init( orca::DriveBicyclePrx &objectPrx )
            {
                logWriter_.write( objectPrx->getDescription() );
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
        LaserScanner2dAutoLogger( const orcalog::LogWriterInfo &logWriterInfo )
            : logWriter_(logWriterInfo)
            {}
        virtual ~LaserScanner2dAutoLogger() {}

        virtual void setData(const orca::RangeScanner2dDataPtr& data, const Ice::Current&)
            { 
                // special case: do not copy to other loggers!
                // we assume that the data is really LaserScanner2dDataPtr but it has to be cast
                // @todo: what if it's not the right type?
                orca::LaserScanner2dDataPtr laserData = orca::LaserScanner2dDataPtr::dynamicCast( data );
                logWriter_.write(laserData);
            }

        virtual void initAndStartLogging()
            {
                orca::LaserScanner2dPrx objectPrx;
                orcaice::connectToInterfaceWithTag( logWriter_.logWriterInfo().context,
                                                    objectPrx,
                                                    logWriter_.logWriterInfo().interfaceTag );

                orca::RangeScanner2dDescription descr = objectPrx->getDescription();
                logWriter_.write( descr );
    
                Ice::ObjectPtr consumer = this;
                orca::RangeScanner2dConsumerPrx callbackPrx = 
                    orcaice::createConsumerInterface<orca::RangeScanner2dConsumerPrx>( logWriter_.logWriterInfo().context,
                                                                                       consumer );

                objectPrx->subscribe( callbackPrx );
            }
    private:
        LaserScanner2dLogWriter logWriter_;
    };

    //////////////////////////////////////////////////////////////////////

    class Localise2dAutoLogger : public GenericAutoLogger<orca::Localise2dData,
                                                            orca::Localise2dConsumer,
                                                            orca::Localise2dConsumerPrx,
                                                            orca::Localise2dPrx,
                                                            Localise2dLogWriter>
    {
    public:
        Localise2dAutoLogger( const orcalog::LogWriterInfo &logWriterInfo )
            : GenericAutoLogger<orca::Localise2dData,
                                orca::Localise2dConsumer,
                                orca::Localise2dConsumerPrx,
                                orca::Localise2dPrx,
                                Localise2dLogWriter>( logWriterInfo )
            {}

        void init( orca::Localise2dPrx &objectPrx )
            {
                logWriter_.write( objectPrx->getVehicleGeometry() );
            }
    };

    //////////////////////////////////////////////////////////////////////

    class Localise3dAutoLogger : public GenericAutoLogger<orca::Localise3dData,
                                                            orca::Localise3dConsumer,
                                                            orca::Localise3dConsumerPrx,
                                                            orca::Localise3dPrx,
                                                            Localise3dLogWriter>
    {
    public:
        Localise3dAutoLogger( const orcalog::LogWriterInfo &logWriterInfo )
            : GenericAutoLogger<orca::Localise3dData,
                                orca::Localise3dConsumer,
                                orca::Localise3dConsumerPrx,
                                orca::Localise3dPrx,
                                Localise3dLogWriter>( logWriterInfo )
            {}

        void init( orca::Localise3dPrx &objectPrx )
            {
                logWriter_.write( objectPrx->getVehicleGeometry() );
            }
    };

    //////////////////////////////////////////////////////////////////////

    class Odometry2dAutoLogger : public GenericAutoLogger<orca::Odometry2dData,
                                                            orca::Odometry2dConsumer,
                                                            orca::Odometry2dConsumerPrx,
                                                            orca::Odometry2dPrx,
                                                            Odometry2dLogWriter>
    {
    public:
        Odometry2dAutoLogger( const orcalog::LogWriterInfo &logWriterInfo )
            : GenericAutoLogger<orca::Odometry2dData,
                                orca::Odometry2dConsumer,
                                orca::Odometry2dConsumerPrx,
                                orca::Odometry2dPrx,
                                Odometry2dLogWriter>( logWriterInfo )
            {}

        void init( orca::Odometry2dPrx &objectPrx )
            {
                logWriter_.write( objectPrx->getDescription() );
            }
    };

    //////////////////////////////////////////////////////////////////////

    class Odometry3dAutoLogger : public GenericAutoLogger<orca::Odometry3dData,
                                                            orca::Odometry3dConsumer,
                                                            orca::Odometry3dConsumerPrx,
                                                            orca::Odometry3dPrx,
                                                            Odometry3dLogWriter>
    {
    public:
        Odometry3dAutoLogger( const orcalog::LogWriterInfo &logWriterInfo )
            : GenericAutoLogger<orca::Odometry3dData,
                                orca::Odometry3dConsumer,
                                orca::Odometry3dConsumerPrx,
                                orca::Odometry3dPrx,
                                Odometry3dLogWriter>( logWriterInfo )
            {}

        void init( orca::Odometry3dPrx &objectPrx )
            {
                logWriter_.write( objectPrx->getDescription() );
            }
    };

    //////////////////////////////////////////////////////////////////////

    typedef GenericAutoLogger<orca::PolarFeature2dDataPtr,
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
    public:
        GpsAutoLogger( const orcalog::LogWriterInfo &logWriterInfo )
            : GenericAutoLogger<orca::GpsData,
                                orca::GpsConsumer,
                                orca::GpsConsumerPrx,
                                orca::GpsPrx,
                                GpsLogWriter>( logWriterInfo )
            {}

        void init( orca::GpsPrx &objectPrx )
            {
                logWriter_.write( objectPrx->getDescription() );
            }
    };

}

#endif
