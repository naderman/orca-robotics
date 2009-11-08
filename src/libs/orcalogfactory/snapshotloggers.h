#ifndef ORCALOGFACTORY_SNAPSHOTLOGGERS_H
#define ORCALOGFACTORY_SNAPSHOTLOGGERS_H

#include <orcaifaceimpl/consumerImpl.h>
#include <IceUtil/Mutex.h>
#include <orcalog/snapshotloggerfactory.h>
#include <orcalogfactory/logwriters.h>
#include <orcaifaceimpl/pathfollower2d.h>

namespace orcalogfactory {

//
// A set of SnapshotLoggers: ie loggers which run continuously after
//                           initialisation, buffering up data, then dump the buffer on command.
//

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericSnapshotLogger<orca::Cpu, orca::CpuConsumer, orca::CpuData,
                                       CpuLogWriter> CpuSnapshotLogger;

//////////////////////////////////////////////////////////////////////

class DriveBicycleSnapshotLogger : public orcalog::GenericSnapshotLogger<orca::DriveBicycle,
                                                                         orca::DriveBicycleConsumer,
                                                                         orca::DriveBicycleData,
                                                                         DriveBicycleLogWriter>
{
public:
    DriveBicycleSnapshotLogger( const orcaice::Context &context ) :
        orcalog::GenericSnapshotLogger<orca::DriveBicycle,
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

typedef orcalog::GenericSnapshotLogger<orca::Imu, orca::ImuConsumer, orca::ImuData,
                          ImuLogWriter> ImuSnapshotLogger;

//////////////////////////////////////////////////////////////////////

class Localise2dSnapshotLogger : public orcalog::GenericSnapshotLogger<orca::Localise2d,
                                                                       orca::Localise2dConsumer,
                                                                       orca::Localise2dData,
                                                                       Localise2dLogWriter>
{
public:
    Localise2dSnapshotLogger( const orcaice::Context &context ) :
        orcalog::GenericSnapshotLogger<orca::Localise2d,
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

class Localise3dSnapshotLogger : public orcalog::GenericSnapshotLogger<orca::Localise3d,
                                                                       orca::Localise3dConsumer,
                                                                       orca::Localise3dData,
                                                                       Localise3dLogWriter>
{
public:
    Localise3dSnapshotLogger( const orcaice::Context &context ) :
        orcalog::GenericSnapshotLogger<orca::Localise3d,
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

class Odometry2dSnapshotLogger : public orcalog::GenericSnapshotLogger<orca::Odometry2d,
                                                                       orca::Odometry2dConsumer,
                                                                       orca::Odometry2dData,
                                                                       Odometry2dLogWriter>
{
public:
    Odometry2dSnapshotLogger( const orcaice::Context &context ) :
        orcalog::GenericSnapshotLogger<orca::Odometry2d,
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

class Odometry3dSnapshotLogger : public orcalog::GenericSnapshotLogger<orca::Odometry3d,
                                                                       orca::Odometry3dConsumer,
                                                                       orca::Odometry3dData,
                                                                       Odometry3dLogWriter>
{
public:
    Odometry3dSnapshotLogger( const orcaice::Context &context ) :
        orcalog::GenericSnapshotLogger<orca::Odometry3d,
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

typedef orcalog::GenericSnapshotLogger<orca::PolarFeature2d, orca::PolarFeature2dConsumer, orca::PolarFeature2dData,
                                       PolarFeature2dLogWriter> PolarFeature2dSnapshotLogger;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericSnapshotLogger<orca::Power, orca::PowerConsumer, orca::PowerData,
                                       PowerLogWriter> PowerSnapshotLogger;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericSnapshotLogger<orca::Wifi, orca::WifiConsumer, orca::WifiData,
                                       WifiLogWriter> WifiSnapshotLogger;

//////////////////////////////////////////////////////////////////////

class GpsSnapshotLogger : public orcalog::GenericSnapshotLogger<orca::Gps,
                                                                orca::GpsConsumer,
                                                                orca::GpsData,
                                                                GpsLogWriter>
{
public:
    GpsSnapshotLogger( const orcaice::Context &context ) :
        orcalog::GenericSnapshotLogger<orca::Gps,
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

//////////////////////////////////////////////////////////////////////

class PathFollower2dSnapshotLogger : 
              public orcaifaceimpl::ConsumerImpl<orca::PathFollower2d,
                                                 orca::PathFollower2dConsumer,
                                                 orca::PathFollower2dData,
                                                 orcaifaceimpl::PathFollower2dConsumerTypeI>, 
              public orcalog::SnapshotLogger
{
public:

    PathFollower2dSnapshotLogger( const orcaice::Context &context );

    virtual void dataEvent(const orca::PathFollower2dData& data );

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
    std::auto_ptr< orcalog::SnapshotLogBuffer<orca::PathFollower2dData,
                                              PathFollower2dLogWriter> > snapshotLogBuffer_;
    std::auto_ptr<PathFollower2dLogWriter>    logWriter_;
    IceUtil::Mutex                            mutex_;

};


} // namespace

#endif
