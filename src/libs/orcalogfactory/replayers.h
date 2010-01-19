#ifndef ORCALOGFACTORY_REPLAYERS_H
#define ORCALOGFACTORY_REPLAYERS_H

#include <orcalog/orcalog.h>
#include <orcalogfactory/logreaders.h>

#include <orcaifaceimpl/camera.h>
#include <orcaifaceimpl/drivebicycle.h>
#include <orcaifaceimpl/gps.h>
#include <orcaifaceimpl/laserscanner2d.h>
#include <orcaifaceimpl/rangescanner2d.h>
#include <orcaifaceimpl/localise2d.h>
#include <orcaifaceimpl/localise3d.h>
#include <orcaifaceimpl/odometry2d.h>
#include <orcaifaceimpl/odometry3d.h>
#include <orcaifaceimpl/power.h>
#include <orcaifaceimpl/wifi.h>
#include <orcaifaceimpl/multicamera.h>
#include <orcaifaceimpl/pathfollower2d.h>

namespace orcalogfactory {

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericReplayer< orcaifaceimpl::CameraImpl,
                                  CameraLogReader,
                                  orca::ImageDataPtr,
                                  orcalog::DescriptionIfaceImplSetup<CameraLogReader,
                                                                     orcaifaceimpl::CameraImpl,
                                                                     orca::CameraDescriptionPtr> >       CameraReplayer;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericReplayer< orcaifaceimpl::DriveBicycleImpl,
                                  DriveBicycleLogReader,
                                  orca::DriveBicycleData,
                                  orcalog::DescriptionIfaceImplSetup<DriveBicycleLogReader,
                                                                     orcaifaceimpl::DriveBicycleImpl,
                                                                     orca::VehicleDescription> >       DriveBicycleReplayer;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericReplayer< orcaifaceimpl::GpsImpl,
                                  GpsLogReader,
                                  orca::GpsData,
                                  orcalog::DescriptionIfaceImplSetup<GpsLogReader,
                                                                     orcaifaceimpl::GpsImpl,
                                                                     orca::GpsDescription> >       GpsReplayer;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericReplayer< orcaifaceimpl::LaserScanner2dImpl,
                                  LaserScanner2dLogReader,
                                  orca::LaserScanner2dDataPtr,
                                  orcalog::DescriptionIfaceImplSetup<LaserScanner2dLogReader,
                                                                     orcaifaceimpl::LaserScanner2dImpl,
                                                                     orca::RangeScanner2dDescription> > LaserScanner2dReplayer;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericReplayer< orcaifaceimpl::RangeScanner2dImpl,
                                  RangeScanner2dLogReader,
                                  orca::RangeScanner2dDataPtr,
                                  orcalog::DescriptionIfaceImplSetup<RangeScanner2dLogReader,
                                                                     orcaifaceimpl::RangeScanner2dImpl,
                                                                     orca::RangeScanner2dDescription> > RangeScanner2dReplayer;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericReplayer< orcaifaceimpl::Localise2dImpl,
                                  Localise2dLogReader,
                                  orca::Localise2dData,
                                  orcalog::DescriptionIfaceImplSetup<Localise2dLogReader,
                                                                     orcaifaceimpl::Localise2dImpl,
                                                                     orca::VehicleGeometryDescriptionPtr> >    Localise2dReplayer;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericReplayer< orcaifaceimpl::Localise3dImpl,
                                  Localise3dLogReader,
                                  orca::Localise3dData,
                                  orcalog::DescriptionIfaceImplSetup<Localise3dLogReader,
                                                                     orcaifaceimpl::Localise3dImpl,
                                                                     orca::VehicleGeometryDescriptionPtr> >    Localise3dReplayer;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericReplayer< orcaifaceimpl::Odometry2dImpl,
                                  Odometry2dLogReader,
                                  orca::Odometry2dData,
                                  orcalog::DescriptionIfaceImplSetup<Odometry2dLogReader,
                                                                     orcaifaceimpl::Odometry2dImpl,
                                                                     orca::VehicleDescription> >    Odometry2dReplayer;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericReplayer< orcaifaceimpl::Odometry3dImpl,
                                  Odometry3dLogReader,
                                  orca::Odometry3dData,
                                  orcalog::DescriptionIfaceImplSetup<Odometry3dLogReader,
                                                                     orcaifaceimpl::Odometry3dImpl,
                                                                     orca::VehicleDescription> >    Odometry3dReplayer;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericReplayer<orcaifaceimpl::PowerImpl,
                                 PowerLogReader,
                                 orca::PowerData> PowerReplayer;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericReplayer<orcaifaceimpl::WifiImpl,
                                 WifiLogReader,
                                 orca::WifiData> WifiReplayer;

//////////////////////////////////////////////////////////////////////

typedef orcalog::GenericReplayer< orcaifaceimpl::MultiCameraImpl,
                                  MultiCameraLogReader,
                                  orca::MultiCameraDataPtr,
                                  orcalog::DescriptionIfaceImplSetup<MultiCameraLogReader,
                                                                     orcaifaceimpl::MultiCameraImpl,
                                                                     orca::MultiCameraDescriptionPtr> >       MultiCameraReplayer;

//////////////////////////////////////////////////////////////////////
//
// This one is non-standard because the interface is non-standard.
//
class NullPathFollower2dCallback : public orcaifaceimpl::AbstractPathFollowerCallback
{
public:
    void setData( const orca::PathFollower2dData &path, bool activateImmediately )
        {
            std::cout<<"TRACE(replayers.h): Ignoring call: " << __func__ << std::endl;
        }
    void activateNow()
        {
            std::cout<<"TRACE(replayers.h): Ignoring call: " << __func__ << std::endl;
        }
    void setEnabled( bool enabled )
        {
            std::cout<<"TRACE(replayers.h): Ignoring call: " << __func__ << std::endl;
        }
    orca::PathFollower2dState getState()
        {
            std::cout<<"TRACE(replayers.h): Ignoring call: " << __func__ << std::endl;
            orca::PathFollower2dState state; return state;
        }
};

class PathFollower2dReplayer : public orcalog::Replayer
{
public:
    PathFollower2dReplayer( const orcalog::LogReaderInfo &logReaderInfo )
        : logReader_( logReaderInfo )
        {}

    void init()
        {
            logReader_.init();
            // IfaceImplSetup( logReader_, iface_ );
            iface_ = new orcaifaceimpl::PathFollower2dImpl( callback_,
                                                            logReader_.logReaderInfo().context,
                                                            logReader_.logReaderInfo().interfaceName );
            iface_->initInterface();
        }

    void replayData( int index )
        {
            logReader_.setLogIndex( index );
            logReader_.read( data_ );
            iface_->localSend( data_ );
        }

    std::string toString() const { return logReader_.toString(); }

protected:

    NullPathFollower2dCallback callback_;
    // IceUtil::Handle<IfaceImplType> iface_;
    orcaifaceimpl::PathFollower2dImplPtr    iface_;
    PathFollower2dLogReader        logReader_;
    orca::PathFollower2dData       data_;
};

//////////////////////////////////////////////////////////////////////

}

#endif
