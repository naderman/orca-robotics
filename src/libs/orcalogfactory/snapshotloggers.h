#ifndef ORCALOGFACTORY_SNAPSHOTLOGGERS_H
#define ORCALOGFACTORY_SNAPSHOTLOGGERS_H

#include <orcaifaceimpl/consumerImpl.h>
#include <IceUtil/Mutex.h>
#include <orcalog/snapshotloggerfactory.h>
#include <orcalog/snapshotlogbuffer.h>
#include <orcalogfactory/logwriters.h>

namespace orcalogfactory {

//
// A set of SnapshotLoggers: ie loggers which run continuously as soon as they're
//                           initialised
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

//////////////////////////////////////////////////////////////////////

typedef GenericSnapshotLogger<orca::Cpu, orca::CpuConsumer, orca::CpuData,
                          CpuLogWriter> CpuSnapshotLogger;

//////////////////////////////////////////////////////////////////////

class DriveBicycleSnapshotLogger : public GenericSnapshotLogger<orca::DriveBicycle,
                                                        orca::DriveBicycleConsumer,
                                                        orca::DriveBicycleData,
                                                        DriveBicycleLogWriter>
{
public:
    DriveBicycleSnapshotLogger( const orcaice::Context &context ) :
        GenericSnapshotLogger<orca::DriveBicycle,
                              orca::DriveBicycleConsumer,
                              orca::DriveBicycleData,
                              DriveBicycleLogWriter>( context ) {};
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

typedef GenericSnapshotLogger<orca::Imu, orca::ImuConsumer, orca::ImuData,
                          ImuLogWriter> ImuSnapshotLogger;

//////////////////////////////////////////////////////////////////////

class Localise2dSnapshotLogger : public GenericSnapshotLogger<orca::Localise2d,
                                                      orca::Localise2dConsumer,
                                                      orca::Localise2dData,
                                                      Localise2dLogWriter>
{
public:
    Localise2dSnapshotLogger( const orcaice::Context &context ) :
        GenericSnapshotLogger<orca::Localise2d,
                              orca::Localise2dConsumer,
                              orca::Localise2dData,
                              Localise2dLogWriter>( context ) {};
private:
    void doSpecialisedSnapshotPreparation( Localise2dLogWriter &logWriter )
        {
            // may throw
            orca::Localise2dPrx objectPrx;
            orcaice::connectToInterfaceWithTag( logWriter.logWriterInfo().context,
                                                objectPrx,
                                                logWriter.logWriterInfo().interfaceTag );
            logWriter.write( objectPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

class Localise3dSnapshotLogger : public GenericSnapshotLogger<orca::Localise3d,
                                                        orca::Localise3dConsumer,
                                                        orca::Localise3dData,
                                                        Localise3dLogWriter>
{
public:
    Localise3dSnapshotLogger( const orcaice::Context &context ) :
        GenericSnapshotLogger<orca::Localise3d,
                              orca::Localise3dConsumer,
                              orca::Localise3dData,
                              Localise3dLogWriter>( context ) {};
private:
    void doSpecialisedSnapshotPreparation( Localise3dLogWriter &logWriter )
        {
            // may throw
            orca::Localise3dPrx objectPrx;
            orcaice::connectToInterfaceWithTag( logWriter.logWriterInfo().context,
                                                objectPrx,
                                                logWriter.logWriterInfo().interfaceTag );
            logWriter.write( objectPrx->getDescription() );
        }
};

//////////////////////////////////////////////////////////////////////

class Odometry2dSnapshotLogger : public GenericSnapshotLogger<orca::Odometry2d,
                                                        orca::Odometry2dConsumer,
                                                        orca::Odometry2dData,
                                                        Odometry2dLogWriter>
{
public:
    Odometry2dSnapshotLogger( const orcaice::Context &context ) :
        GenericSnapshotLogger<orca::Odometry2d,
                              orca::Odometry2dConsumer,
                              orca::Odometry2dData,
                              Odometry2dLogWriter>( context ) {};
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

class Odometry3dSnapshotLogger : public GenericSnapshotLogger<orca::Odometry3d,
                                                        orca::Odometry3dConsumer,
                                                        orca::Odometry3dData,
                                                        Odometry3dLogWriter>
{
public:
    Odometry3dSnapshotLogger( const orcaice::Context &context ) :
        GenericSnapshotLogger<orca::Odometry3d,
                              orca::Odometry3dConsumer,
                              orca::Odometry3dData,
                              Odometry3dLogWriter>( context ) {};
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

typedef GenericSnapshotLogger<orca::PolarFeature2d, orca::PolarFeature2dConsumer, orca::PolarFeature2dData,
                          PolarFeature2dLogWriter> PolarFeature2dSnapshotLogger;

//////////////////////////////////////////////////////////////////////

typedef GenericSnapshotLogger<orca::Power, orca::PowerConsumer, orca::PowerData,
                          PowerLogWriter> PowerSnapshotLogger;

//////////////////////////////////////////////////////////////////////

typedef GenericSnapshotLogger<orca::Wifi, orca::WifiConsumer, orca::WifiData,
                          WifiLogWriter> WifiSnapshotLogger;

//////////////////////////////////////////////////////////////////////

class GpsSnapshotLogger : public GenericSnapshotLogger<orca::Gps,
                                                        orca::GpsConsumer,
                                                        orca::GpsData,
                                                        GpsLogWriter>
{
public:
    GpsSnapshotLogger( const orcaice::Context &context ) :
        GenericSnapshotLogger<orca::Gps,
                              orca::GpsConsumer,
                              orca::GpsData,
                              GpsLogWriter>( context ) {};
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
//////////////////////////////////////////////////////////////////////

class LaserScanner2dSnapshotLogger : 
              public orcaifaceimpl::ConsumerImpl<orca::RangeScanner2d,
                                                orca::RangeScanner2dConsumer,
                                                orca::RangeScanner2dDataPtr>, 
              public orcalog::SnapshotLogger
{
public:

    LaserScanner2dSnapshotLogger( const orcaice::Context &context );

    virtual void dataEvent(const orca::RangeScanner2dDataPtr& data );

    virtual void init( const std::string &format, double timeWindowSec );

    virtual void subscribe( const std::string &interfaceTag )
        { subscribeWithTag( interfaceTag ); };

    virtual void prepareForSnapshot( const orcalog::LogWriterInfo &logWriterInfo,
                                      orcalog::MasterFileWriter    &masterFileWriter );
    virtual void finaliseSnapshot();

    virtual unsigned int snapshotBufferSize() const
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

} // namespace

#endif
