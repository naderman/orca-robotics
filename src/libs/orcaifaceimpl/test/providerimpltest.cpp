#include <iostream>
#include <orcaice/component.h>
#include <orcaice/application.h>
#include <orcaice/orcaice.h>

#include <orcaifaceimpl/button.h>
#include <orcaifaceimpl/camera.h>
#include <orcaifaceimpl/multicamera.h>
#include <orcaifaceimpl/cpu.h>
#include <orcaifaceimpl/drivebicycle.h>
// TODO: currently missing
// #include <orcaifaceimpl/drivedifferential.h>
#include <orcaifaceimpl/estop.h>
#include <orcaifaceimpl/featuremap2d.h>
#include <orcaifaceimpl/gps.h>
#include <orcaifaceimpl/binaryswitch.h>
// Home is an admin interface which does not publish, HomeImpl lives in libOrcaIce
#include <orcaifaceimpl/image.h>
#include <orcaifaceimpl/imu.h>
#include <orcaifaceimpl/ins.h>
#include <orcaifaceimpl/laserscanner2d.h>
#include <orcaifaceimpl/localise2d.h>
#include <orcaifaceimpl/localise3d.h>
#include <orcaifaceimpl/odometry2d.h>
#include <orcaifaceimpl/odometry3d.h>
// TODO: currently missing
// #include <orcaifaceimpl/ogfusion.h>
#include <orcaifaceimpl/ogmap.h>
#include <orcaifaceimpl/particle2d.h>
#include <orcaifaceimpl/pathfollower2d.h>
// TODO: PathPlannerImpl is missing
#include <orcaifaceimpl/pathplanner2d.h>
#include <orcaifaceimpl/pixmap.h>
#include <orcaifaceimpl/pointcloud.h>
#include <orcaifaceimpl/polarfeature2d.h>
#include <orcaifaceimpl/power.h>
#include <orcaifaceimpl/properties.h>
#include <orcaifaceimpl/qgraphics2d.h>
#include <orcaifaceimpl/rangescanner2d.h>
// Status is an admin interface, there is a StatusImpl implimentation here but another one lives in libOrcaIce
#include <orcaifaceimpl/status.h>
#include <orcaifaceimpl/systemstatus.h>
// Tracer is an admin interface, there is no TracerImpl implimentation here, it lives in libOrcaIce
#include <orcaifaceimpl/tracer.h>
#include <orcaifaceimpl/velocitycontrol2d.h>
#include <orcaifaceimpl/wifi.h>

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent() :
        orcaice::Component( "Test", orcaice::NoStandardInterfaces ) {}
    virtual void start();
    virtual void stop() {};
};

class PathFollowerCallback : public orcaifaceimpl::AbstractPathFollowerCallback {
public:
    virtual void setData( const orca::PathFollower2dData &path, bool activateImmediately ) {};
    virtual void activateNow() {};
    virtual int  getWaypointIndex() { return 0; };
    virtual bool getAbsoluteActivationTime( orca::Time &activationTime ) { return true; };
    virtual bool getRelativeActivationTime( double &secondsSinceActivation ) { return true; };
    virtual void setEnabled( bool enabled ) {};
    virtual bool enabled() { return true; };
};

class BinarySwitchCallback : public orcaifaceimpl::AbstractBinarySwitchCallback {
public:
    virtual void setData( const orca::BinarySwitchData &data ) {};
};

void
TestComponent::start()
{
    {
        orcaifaceimpl::ButtonImplPtr ptr = new orcaifaceimpl::ButtonImpl( context(), "crap" );
    }
    {
        orcaifaceimpl::CameraImplPtr ptr = new orcaifaceimpl::CameraImpl( new orca::CameraDescription, context(), "crap" );
        ptr->localSet( new orca::ImageData );
    }
    {
        orcaifaceimpl::CpuImplPtr ptr = new orcaifaceimpl::CpuImpl( orca::CpuDescription(), context(), "crap" );
        ptr->localSet( orca::CpuData() );
    }
    {
        orcaifaceimpl::DriveBicycleImplPtr ptr = new orcaifaceimpl::DriveBicycleImpl( orca::VehicleDescription(), context(), "crap" );
        ptr->localSet( orca::DriveBicycleData() );
    }
    {
        orcaifaceimpl::EStopImplPtr ptr = new orcaifaceimpl::EStopImpl( orca::EStopDescription(), context(), "crap" );
        ptr->localSet( orca::EStopData() );
    }
    {
        orcaifaceimpl::FeatureMap2dImplPtr ptr = new orcaifaceimpl::FeatureMap2dImpl( context(), "crap" );
        ptr->localSet( orca::FeatureMap2dData() );
    }
    {
        orcaifaceimpl::GpsImplPtr ptr = new orcaifaceimpl::GpsImpl( orca::GpsDescription(), context(), "crap" );
        ptr->localSet( orca::GpsData() );
    }
    {
        BinarySwitchCallback cb;
        orcaifaceimpl::BinarySwitchImplPtr ptr = new orcaifaceimpl::BinarySwitchImpl( cb, context(), "crap" );
        ptr->localSet( orca::BinarySwitchData() );
    }
    {
        orcaifaceimpl::ImageImplPtr ptr = new orcaifaceimpl::ImageImpl( new orca::ImageDescription, context(), "crap" );
        ptr->localSet( new orca::ImageData );
    }
    {
        orcaifaceimpl::ImuImplPtr ptr = new orcaifaceimpl::ImuImpl( orca::ImuDescription(), context(), "crap" );
        ptr->localSet( orca::ImuData() );
    }
    {
        orcaifaceimpl::InsImplPtr ptr = new orcaifaceimpl::InsImpl( orca::InsDescription(), context(), "crap" );
        ptr->localSet( orca::InsData() );
    }
    {
        orcaifaceimpl::LaserScanner2dImplPtr ptr = new orcaifaceimpl::LaserScanner2dImpl( orca::RangeScanner2dDescription(), context(), "crap" );
        ptr->localSet( new orca::LaserScanner2dData );
    }
    {
        orcaifaceimpl::Localise2dImplPtr ptr = new orcaifaceimpl::Localise2dImpl( new orca::VehicleGeometryDescription, context(), "crap" );
        ptr->localSet( orca::Localise2dData() );
    }
    {
        orcaifaceimpl::Localise3dImplPtr ptr = new orcaifaceimpl::Localise3dImpl( new orca::VehicleGeometryDescription, context(), "crap" );
        ptr->localSet( orca::Localise3dData() );
    }
    {
        orcaifaceimpl::MultiCameraImplPtr ptr = new orcaifaceimpl::MultiCameraImpl( new orca::MultiCameraDescription, context(), "crap" );
        ptr->localSet( new orca::MultiCameraData );
    }
    {
        orcaifaceimpl::Odometry2dImplPtr ptr = new orcaifaceimpl::Odometry2dImpl( orca::VehicleDescription(), context(), "crap" );
        ptr->localSet( orca::Odometry2dData() );
    }
    {
        orcaifaceimpl::Odometry3dImplPtr ptr = new orcaifaceimpl::Odometry3dImpl( orca::VehicleDescription(), context(), "crap" );
        ptr->localSet( orca::Odometry3dData() );
    }
    {
        orcaifaceimpl::OgMapImplPtr ptr = new orcaifaceimpl::OgMapImpl( context(), "crap" );
        ptr->localSet( orca::OgMapData() );
    }
    {
        orcaifaceimpl::Particle2dImplPtr ptr = new orcaifaceimpl::Particle2dImpl( context(), "crap" );
        ptr->localSet( orca::Particle2dData() );
    }
    {
        PathFollowerCallback cb;
        orcaifaceimpl::PathFollower2dImplPtr ptr = new orcaifaceimpl::PathFollower2dImpl( cb, context(), "crap" );
//         ptr->localSet( orca::PathFollower2dData() );
    }
    {
        orcaifaceimpl::PixMapImplPtr ptr = new orcaifaceimpl::PixMapImpl( context(), "crap" );
        ptr->localSet( orca::PixMapData() );
    }
    {
        orcaifaceimpl::PointCloudImplPtr ptr = new orcaifaceimpl::PointCloudImpl( orca::PointCloudDescription(), context(), "crap" );
        ptr->localSet( orca::PointCloudData() );
    }
    {
        orcaifaceimpl::PolarFeature2dImplPtr ptr = new orcaifaceimpl::PolarFeature2dImpl( orca::PolarFeature2dDescription(), context(), "crap" );
        ptr->localSet( orca::PolarFeature2dData() );
    }
    {
        orcaifaceimpl::PowerImplPtr ptr = new orcaifaceimpl::PowerImpl( context(), "crap" );
        ptr->localSet( orca::PowerData() );
    }
    {
        orcaifaceimpl::PropertiesImplPtr ptr = new orcaifaceimpl::PropertiesImpl( context(), "crap" );
        ptr->localSet( orca::PropertiesData() );
    }
    {
        orcaifaceimpl::QGraphics2dImplPtr ptr = new orcaifaceimpl::QGraphics2dImpl( context(), "crap" );
        ptr->localSet( orca::QGraphics2dData() );
    }
    {
        orcaifaceimpl::RangeScanner2dImplPtr ptr = new orcaifaceimpl::RangeScanner2dImpl( orca::RangeScanner2dDescription(), context(), "crap" );
        ptr->localSet( new orca::RangeScanner2dData );
    }
    {
        orcaifaceimpl::StatusImplPtr ptr = new orcaifaceimpl::StatusImpl( context(), "crap" );
        ptr->localSet( orca::StatusData() );
    }
    {
        orcaifaceimpl::SystemStatusImplPtr ptr = new orcaifaceimpl::SystemStatusImpl( context(), "crap" );
        ptr->localSet( orca::SystemStatusData() );
    }
//     {
//          NOTE: TracerImpl is only used in libOrcaIce, that's where lives.
//                (Tracer interface does not fit into any of the templates)
//     }
    {
        orcaifaceimpl::VelocityControl2dImplPtr ptr = new orcaifaceimpl::VelocityControl2dImpl( orca::VehicleDescription(), context(), "crap" );
    }
    {
        orcaifaceimpl::WifiImplPtr ptr = new orcaifaceimpl::WifiImpl( context(), "crap" );
        ptr->localSet( orca::WifiData() );
    }

    // NOTE: cannot call communicator()->destroy() from here
    // because they'll be caught by Ice::Application and show up as failed ctest.
    exit(EXIT_SUCCESS);
}

//
// Build the component into a stand-alone application
//
int
main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.orcaMain(argc, argv);
}
