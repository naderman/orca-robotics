#ifndef ORCALOGFACTORY_REPLAYERS_H
#define ORCALOGFACTORY_REPLAYERS_H

#include <orcalog/orcalog.h>
#include <orcalogfactory/logreaders.h>

#include <orcaifaceimpl/drivebicycleImpl.h>
#include <orcaifaceimpl/gpsImpl.h>
#include <orcaifaceimpl/laserscanner2dImpl.h>
#include <orcaifaceimpl/localise2dImpl.h>
#include <orcaifaceimpl/localise3dImpl.h>
#include <orcaifaceimpl/odometry2dImpl.h>
#include <orcaifaceimpl/odometry3dImpl.h>
#include <orcaifaceimpl/powerImpl.h>
#include <orcaifaceimpl/wifiImpl.h>

namespace orcalogfactory {

// fwd declarations (ignore, they're defined below)
template<class LogReaderType,
         class IfaceImplType>
class DefaultIfaceImplSetup;
template<class LogReaderType,
         class IfaceImplType,
         class DescriptionType>
class DescriptionIfaceImplSetup;


//////////////////////////////////////////////////////////////////////


//
// Generic Replayer.  The last template argument is a class whose constructor
// will set up the IfaceImpl (including reading a description if necessary).
//
template< class IfaceImplType,
          class LogReaderType,
          class DataType,
          class IfaceImplSetup=DefaultIfaceImplSetup<LogReaderType,IfaceImplType> >
class GenericReplayer : public orcalog::Replayer
{
public:

    GenericReplayer( const orcalog::LogReaderInfo &logReaderInfo )
        : logReader_( logReaderInfo )
        {}
    virtual ~GenericReplayer() {}

    void init()
        {
            logReader_.init();
            IfaceImplSetup( logReader_, iface_ );
            iface_->initInterface();
        }

    void replayData( int index )
        {
            logReader_.setLogIndex( index );
            logReader_.read( data_ );
            iface_->localSetAndSend( data_ );
        }

    std::string toString() const { return logReader_.toString(); }

protected:

    IceUtil::Handle<IfaceImplType> iface_;
    LogReaderType                  logReader_;
    DataType                       data_;

};

//
// Useful defaults for setting up the IfaceImpl:
//

//
// This default will set up the IfaceImpl with no description required.
//
template<class LogReaderType,
         class IfaceImplType>
class DefaultIfaceImplSetup {
public:
    DefaultIfaceImplSetup( LogReaderType                  &logReader,
                           IceUtil::Handle<IfaceImplType> &iface )
        {
            iface = new IfaceImplType( logReader.logReaderInfo().context,
                                       logReader.logReaderInfo().interfaceName );
        }
};
//
// This version works when all that needs to happen is that a description
// needs to be read in.
//
template<class LogReaderType,
         class IfaceImplType,
         class DescriptionType>
class DescriptionIfaceImplSetup {
public:
    DescriptionIfaceImplSetup( LogReaderType                  &logReader,
                               IceUtil::Handle<IfaceImplType> &iface )
        {
            DescriptionType descr;
            logReader.read( descr );
            iface = new IfaceImplType( descr,
                                       logReader.logReaderInfo().context,
                                       logReader.logReaderInfo().interfaceName );
        }
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

typedef GenericReplayer< orcaifaceimpl::DriveBicycleImpl,
                         DriveBicycleLogReader,
                         orca::DriveBicycleData,
                         DescriptionIfaceImplSetup<DriveBicycleLogReader,
                                                   orcaifaceimpl::DriveBicycleImpl,
                                                   orca::VehicleDescription> >       DriveBicycleReplayer;

//////////////////////////////////////////////////////////////////////

typedef GenericReplayer< orcaifaceimpl::GpsImpl,
                         GpsLogReader,
                         orca::GpsData,
                         DescriptionIfaceImplSetup<GpsLogReader,
                                                   orcaifaceimpl::GpsImpl,
                                                   orca::GpsDescription> >       GpsReplayer;

//////////////////////////////////////////////////////////////////////

typedef GenericReplayer< orcaifaceimpl::LaserScanner2dImpl,
                         LaserScanner2dLogReader,
                         orca::LaserScanner2dDataPtr,
                         DescriptionIfaceImplSetup<LaserScanner2dLogReader,
                                                  orcaifaceimpl::LaserScanner2dImpl,
                                                  orca::RangeScanner2dDescription> > LaserScanner2dReplayer;

//////////////////////////////////////////////////////////////////////

typedef GenericReplayer< orcaifaceimpl::Localise2dImpl,
                         Localise2dLogReader,
                         orca::Localise2dData,
                         DescriptionIfaceImplSetup<Localise2dLogReader,
                                                  orcaifaceimpl::Localise2dImpl,
                                                  orca::VehicleGeometryDescriptionPtr> >    Localise2dReplayer;

//////////////////////////////////////////////////////////////////////

typedef GenericReplayer< orcaifaceimpl::Localise3dImpl,
                         Localise3dLogReader,
                         orca::Localise3dData,
                         DescriptionIfaceImplSetup<Localise3dLogReader,
                                                  orcaifaceimpl::Localise3dImpl,
                                                  orca::VehicleGeometryDescriptionPtr> >    Localise3dReplayer;

//////////////////////////////////////////////////////////////////////

typedef GenericReplayer< orcaifaceimpl::Odometry2dImpl,
                         Odometry2dLogReader,
                         orca::Odometry2dData,
                         DescriptionIfaceImplSetup<Odometry2dLogReader,
                                                  orcaifaceimpl::Odometry2dImpl,
                                                  orca::VehicleDescription> >    Odometry2dReplayer;

//////////////////////////////////////////////////////////////////////

typedef GenericReplayer< orcaifaceimpl::Odometry3dImpl,
                         Odometry3dLogReader,
                         orca::Odometry3dData,
                         DescriptionIfaceImplSetup<Odometry3dLogReader,
                                                  orcaifaceimpl::Odometry3dImpl,
                                                  orca::VehicleDescription> >    Odometry3dReplayer;

//////////////////////////////////////////////////////////////////////

typedef GenericReplayer<orcaifaceimpl::PowerImpl,
                        PowerLogReader,
                        orca::PowerData> PowerReplayer;

//////////////////////////////////////////////////////////////////////

typedef GenericReplayer<orcaifaceimpl::WifiImpl,
                        WifiLogReader,
                        orca::WifiData> WifiReplayer;

}

#endif




// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//
// AlexB: A slightly different approach that I tried and left lying around
//        just in case.
//

// template<class IfaceImplType,
//          class LogReaderType,
//          class DataType>
// class GenericReplayer : public orcalog::Replayer
// {
// public:

//     GenericReplayer( const orcalog::LogReaderInfo &logReaderInfo )
//         : logReader_( logReaderInfo )
//         {}
//     virtual ~GenericReplayer() {}

//     void init()
//         {
//             logReader_.init();
//             setupIface();
//             iface_->initInterface();
//         }

//     void replayData( int index )
//         {
//             logReader_.setLogIndex( index );
//             logReader_.read( data_ );
//             iface_->localSetAndSend( data_ );
//         }

//     std::string toString() const { return logReader_.toString(); }

// protected:

//     // Derived classes should overload.
//     virtual void setupIface()=0;

//     IceUtil::Handle<IfaceImplType> iface_;
//     LogReaderType                  logReader_;
//     DataType                       data_;

// };

// //////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////

// class LaserScanner2dReplayer : public GenericReplayer<orcaifaceimpl::LaserScanner2dImpl,
//                                                       LaserScanner2dLogReader,
//                                                       orca::LaserScanner2dDataPtr>
// {
// public:

//     LaserScanner2dReplayer( const orcalog::LogReaderInfo &logReaderInfo )
//         : GenericReplayer<orcaifaceimpl::LaserScanner2dImpl,
//                           LaserScanner2dLogReader,
//                           orca::LaserScanner2dDataPtr>( logReaderInfo )
//         {}

// private:

//     virtual void setupIface()
//         {
//             orca::RangeScanner2dDescription descr;
//             logReader_.read( descr );
//             iface_ = new orcaifaceimpl::LaserScanner2dImpl( descr,
//                                                             logReader_.logReaderInfo().context,
//                                                             logReader_.logReaderInfo().interfaceName );
//         }
// };

// //////////////////////////////////////////////////////////////////////

// class PowerReplayer : public GenericReplayer<orcaifaceimpl::PowerImpl,
//                                              PowerLogReader,
//                                              orca::PowerData>
// {
// public:

//     PowerReplayer( const orcalog::LogReaderInfo &logReaderInfo )
//         : GenericReplayer<orcaifaceimpl::PowerImpl,
//                           PowerLogReader,
//                           orca::PowerData>( logReaderInfo )
//         {}

// private:

//     virtual void setupIface()
//         {
//             iface_ = new orcaifaceimpl::PowerImpl( logReader_.logReaderInfo().context,
//                                                    logReader_.logReaderInfo().interfaceName );
//         }
// };

// // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

