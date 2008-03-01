#ifndef ORCALOGFACTORY_SNAPSHOTLOGGERS_H
#define ORCALOGFACTORY_SNAPSHOTLOGGERS_H

#include <IceUtil/Mutex.h>
#include <orcalog/snapshotloggerfactory.h>
#include <orcalog/snapshotlogbuffer.h>
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

        GenericSnapshotLogger()
            : isTakingSnapshot_(false)
            {}
        virtual ~GenericSnapshotLogger() {}

        // Assumes the ConsumerType's method is called setData
        virtual void setData(const DataType& data, const Ice::Current&)
            { 
                {
                    // Don't accept new data while taking a snapshot.
                    IceUtil::Mutex::Lock lock(mutex_);
                    if ( isTakingSnapshot_ ) return;
                }
                snapshotLogBuffer_->addItem( data );
            }

        // Inherited from orcalog::SnapshotLogger
        virtual void init( const std::string &format, double timeWindowSec )
            {
                LogWriterType dummyLogWriter;
                dummyLogWriter.checkFormat( format );
                snapshotLogBuffer_.reset( new orcalog::SnapshotLogBuffer<DataType,
                                                                         LogWriterType>( timeWindowSec ) );
            }

        virtual void subscribe( orcaice::Context &context, const std::string &interfaceTag )
            {
                // may throw
                PrxType objectPrx;
                orcaice::connectToInterfaceWithTag<PrxType>( context,
                                                             objectPrx,
                                                             interfaceTag );

                Ice::ObjectPtr consumer = this;
                ConsumerPrxType callbackPrx = 
                    orcaice::createConsumerInterface<ConsumerPrxType>( context,
                                                                       consumer );

                objectPrx->subscribe( callbackPrx );
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

        virtual uint snapshotBufferSize() const
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
        std::auto_ptr< orcalog::SnapshotLogBuffer<DataType,LogWriterType> > snapshotLogBuffer_;
        std::auto_ptr<LogWriterType>              logWriter_;
        IceUtil::Mutex                            mutex_;
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
        void doSpecialisedSnapshotPreparation( DriveBicycleLogWriter &logWriter )
            {
                // may throw
                orca::DriveBicyclePrx objectPrx;
                orcaice::connectToInterfaceWithTag( logWriter.logWriterInfo().context,
                                                    objectPrx,
                                                    logWriter.logWriterInfo().interfaceTag );
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

        LaserScanner2dSnapshotLogger()
            : isTakingSnapshot_(false)
            {}
        virtual ~LaserScanner2dSnapshotLogger() {}

        virtual void setData(const orca::RangeScanner2dDataPtr& data, const Ice::Current &c)
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

        virtual void init( const std::string &format, double timeWindowSec )
            {
                LaserScanner2dLogWriter dummyLogWriter;
                dummyLogWriter.checkFormat( format );
                snapshotLogBuffer_.reset( new orcalog::SnapshotLogBuffer<orca::LaserScanner2dDataPtr,
                                                                         LaserScanner2dLogWriter>( timeWindowSec ) );
            }

        virtual void subscribe( orcaice::Context &context, const std::string &interfaceTag )
            {
                orca::LaserScanner2dPrx objectPrx;
                orcaice::connectToInterfaceWithTag( context,
                                                    objectPrx,
                                                    interfaceTag );

                Ice::ObjectPtr consumer = this;
                orca::RangeScanner2dConsumerPrx callbackPrx = 
                    orcaice::createConsumerInterface<orca::RangeScanner2dConsumerPrx>( context,
                                                                                       consumer );

                objectPrx->subscribe( callbackPrx );
            }

        virtual void prepareForSnapshot( const orcalog::LogWriterInfo &logWriterInfo,
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
                orca::LaserScanner2dPrx objectPrx;
                orcaice::connectToInterfaceWithTag( logWriter_->logWriterInfo().context,
                                                    objectPrx,
                                                    logWriter_->logWriterInfo().interfaceTag );
                logWriter_->write( objectPrx->getDescription() );
            }
        virtual void finaliseSnapshot()
            {
                {
                    IceUtil::Mutex::Lock lock(mutex_);
                    isTakingSnapshot_ = false;
                }
            }
        virtual uint snapshotBufferSize() const
            { return snapshotLogBuffer_->bufferSize(); }
        virtual const orca::Time &oldestArrivalTime() const
            { return snapshotLogBuffer_->oldestArrivalTime(); }
        virtual void writeOldestObjToLogAndDiscard()
            { snapshotLogBuffer_->writeOldestObjToLogAndDiscard(*logWriter_); }

    private:

        bool                                      isTakingSnapshot_;
        std::auto_ptr< orcalog::SnapshotLogBuffer<orca::LaserScanner2dDataPtr,
                                                  LaserScanner2dLogWriter> > snapshotLogBuffer_;
        std::auto_ptr<LaserScanner2dLogWriter>    logWriter_;
        IceUtil::Mutex                            mutex_;

    };

    //////////////////////////////////////////////////////////////////////

    class Localise2dSnapshotLogger : public GenericSnapshotLogger<orca::Localise2dData,
                                                          orca::Localise2dConsumer,
                                                          orca::Localise2dConsumerPrx,
                                                          orca::Localise2dPrx,
                                                          Localise2dLogWriter>
    {
    private:
        void doSpecialisedSnapshotPreparation( Localise2dLogWriter &logWriter )
            {
                // may throw
                orca::Localise2dPrx objectPrx;
                orcaice::connectToInterfaceWithTag( logWriter.logWriterInfo().context,
                                                    objectPrx,
                                                    logWriter.logWriterInfo().interfaceTag );
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
        void doSpecialisedSnapshotPreparation( Localise3dLogWriter &logWriter )
            {
                // may throw
                orca::Localise3dPrx objectPrx;
                orcaice::connectToInterfaceWithTag( logWriter.logWriterInfo().context,
                                                    objectPrx,
                                                    logWriter.logWriterInfo().interfaceTag );
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
        void doSpecialisedSnapshotPreparation( Odometry2dLogWriter &logWriter )
            {
                // may throw
                orca::Odometry2dPrx objectPrx;
                orcaice::connectToInterfaceWithTag( logWriter.logWriterInfo().context,
                                                    objectPrx,
                                                    logWriter.logWriterInfo().interfaceTag );
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
        void doSpecialisedSnapshotPreparation( Odometry3dLogWriter &logWriter )
            {
                // may throw
                orca::Odometry3dPrx objectPrx;
                orcaice::connectToInterfaceWithTag( logWriter.logWriterInfo().context,
                                                    objectPrx,
                                                    logWriter.logWriterInfo().interfaceTag );
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
        void doSpecialisedSnapshotPreparation( GpsLogWriter &logWriter )
            {
                // may throw
                orca::GpsPrx objectPrx;
                orcaice::connectToInterfaceWithTag( logWriter.logWriterInfo().context,
                                                    objectPrx,
                                                    logWriter.logWriterInfo().interfaceTag );
                logWriter.write( objectPrx->getDescription() );
            }
    };

}

#endif
