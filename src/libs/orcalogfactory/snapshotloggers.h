#ifndef ORCALOGFACTORY_SNAPSHOTLOGGERS_H
#define ORCALOGFACTORY_SNAPSHOTLOGGERS_H

#include <orcalog/snapshotloggerfactory.h>
#include <orcalogfactory/logwriters.h>
#include <orcaice/connectutils.h>

namespace orcalogfactory {

    //
    // A set of SnapshotLoggers: ie loggers which run continuously as soon as they're
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
    class GenericSnapshotLogger : public ConsumerType, public orcalog::SnapshotLogger
    {
    public:

        virtual ~GenericSnapshotLogger() {}

        // Assumes the ConsumerType's method is called setData
        virtual void setData(const DataType& data, const Ice::Current&)
            { logWriter_->write(data); }

        // Inherited from orcalog::SnapshotLogger
        virtual void init( const std::string &format )
            {
                LogWriterType dummyLogWriter;
                dummyLogWriter.checkFormat( format );
            }

        virtual void subscribe( orcaice::Context &context, const std::string &interfaceTag )
            {
                // may throw
                PrxType objectPrx;
                orcaice::connectToInterfaceWithTag<PrxType>( context,
                                                             objectPrx,
                                                             interfaceTag );

                // Allow derived classes to do something special (like get description)
                setup(objectPrx,*logWriter_);
    
                Ice::ObjectPtr consumer = this;
                ConsumerPrxType callbackPrx = 
                    orcaice::createConsumerInterface<ConsumerPrxType>( context,
                                                                       consumer );

                objectPrx->subscribe( callbackPrx );
            }

        virtual void takeSnapshot( const orcalog::LogWriterInfo &logWriterInfo,
                                   orcalog::MasterFileWriter    &masterFileWriter )
            {
                std::cout<<"TRACE(snapshotloggers.h): TODO" << std::endl;
                logWriter_.reset( new LogWriterType );
                logWriter_->init( logWriterInfo, masterFileWriter );
            }

    protected:

        // Called during initialisation
        virtual void setup( PrxType &objectPrx, LogWriterType &logWriter ) {}

    private:

        std::auto_ptr<LogWriterType> logWriter_;
    };

    //////////////////////////////////////////////////////////////////////

    typedef GenericSnapshotLogger<orca::CpuData,
                              orca::CpuConsumer,
                              orca::CpuConsumerPrx,
                              orca::CpuPrx,
                              CpuLogWriter> CpuSnapshotLogger;

    //////////////////////////////////////////////////////////////////////

    class DriveBicycleSnapshotLogger : public GenericSnapshotLogger<orca::DriveBicycleData,
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

    typedef GenericSnapshotLogger<orca::ImuData,
                              orca::ImuConsumer,
                              orca::ImuConsumerPrx,
                              orca::ImuPrx,
                              ImuLogWriter> ImuSnapshotLogger;

    //////////////////////////////////////////////////////////////////////

    class LaserScanner2dSnapshotLogger : public orca::RangeScanner2dConsumer, public orcalog::SnapshotLogger
    {
    public:
        virtual ~LaserScanner2dSnapshotLogger() {}

        virtual void setData(const orca::RangeScanner2dDataPtr& data, const Ice::Current&)
            { 
                // special case: do not copy to other loggers!
                // we assume that the data is really LaserScanner2dDataPtr but it has to be cast
                // @todo: what if it's not the right type?
                orca::LaserScanner2dDataPtr laserData = orca::LaserScanner2dDataPtr::dynamicCast( data );
                logWriter_->write(laserData);
            }

        virtual void init( const std::string &format )
            {
                LaserScanner2dLogWriter dummyLogWriter;
                dummyLogWriter.checkFormat( format );
            }

        virtual void subscribe( orcaice::Context &context, const std::string &interfaceTag )
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

        virtual void takeSnapshot( const orcalog::LogWriterInfo &logWriterInfo,
                                   orcalog::MasterFileWriter &masterFileWriter )
            {
                std::cout<<"TRACE(snapshotloggers.h): TODO" << std::endl;
                logWriter_.reset( new LaserScanner2dLogWriter );
                logWriter_->init( logWriterInfo, masterFileWriter );
            }

    private:
        std::auto_ptr<LaserScanner2dLogWriter> logWriter_;
    };

    //////////////////////////////////////////////////////////////////////

    class Localise2dSnapshotLogger : public GenericSnapshotLogger<orca::Localise2dData,
                                                          orca::Localise2dConsumer,
                                                          orca::Localise2dConsumerPrx,
                                                          orca::Localise2dPrx,
                                                          Localise2dLogWriter>
    {
    private:
        void setup( orca::Localise2dPrx &objectPrx, Localise2dLogWriter &logWriter )
            {
                logWriter.write( objectPrx->getVehicleGeometry() );
            }
    };

    //////////////////////////////////////////////////////////////////////

    class Localise3dSnapshotLogger : public GenericSnapshotLogger<orca::Localise3dData,
                                                            orca::Localise3dConsumer,
                                                            orca::Localise3dConsumerPrx,
                                                            orca::Localise3dPrx,
                                                            Localise3dLogWriter>
    {
    private:
        void setup( orca::Localise3dPrx &objectPrx, Localise3dLogWriter &logWriter )
            {
                logWriter.write( objectPrx->getVehicleGeometry() );
            }
    };

    //////////////////////////////////////////////////////////////////////

    class Odometry2dSnapshotLogger : public GenericSnapshotLogger<orca::Odometry2dData,
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

    class Odometry3dSnapshotLogger : public GenericSnapshotLogger<orca::Odometry3dData,
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

    typedef GenericSnapshotLogger<orca::PolarFeature2dData,
                              orca::PolarFeature2dConsumer,
                              orca::PolarFeature2dConsumerPrx,
                              orca::PolarFeature2dPrx,
                              PolarFeature2dLogWriter> PolarFeature2dSnapshotLogger;

    //////////////////////////////////////////////////////////////////////
    
    typedef GenericSnapshotLogger<orca::PowerData,
                              orca::PowerConsumer,
                              orca::PowerConsumerPrx,
                              orca::PowerPrx,
                              PowerLogWriter> PowerSnapshotLogger;

    //////////////////////////////////////////////////////////////////////

    typedef GenericSnapshotLogger<orca::WifiData,
                              orca::WifiConsumer,
                              orca::WifiConsumerPrx,
                              orca::WifiPrx,
                              WifiLogWriter> WifiSnapshotLogger;

    //////////////////////////////////////////////////////////////////////

    class GpsSnapshotLogger : public GenericSnapshotLogger<orca::GpsData,
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

}

#endif
