/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "component.h"
#include "mainloop.h"

// Various bits of hardware we can drive
#include "driver.h"
#include "fakedriver.h"
#include "sicklaserutil.h"
#ifdef HAVE_CARMEN_DRIVER
#  include "sickcarmen/sickcarmendriver.h"
#endif
#ifdef HAVE_PLAYERCLIENT_DRIVER
#  include "playerclient/playerclientdriver.h"
#endif

// implementations of Ice objects
#include "laserscanner2dI.h"

#include <orcaice/orcaice.h>

namespace laser2d {

using namespace std;
using namespace orca;

Component::Component()
    : orcaice::Component( "Laser2d" ),
      mainLoop_(0),
      hwDriver_(0)
{
}

Component::~Component()
{
    delete hwDriver_;
    
    // do not delete mainLoop_!!! They derive from Ice::Thread and self-destruct.
}

void
Component::start()
{
    //
    // INITIAL CONFIGURATION
    //

    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag();
    prefix += ".Config.";

    // Laser geometry
    orca::RangeScanner2dGeometryPtr geometry = new orca::RangeScanner2dGeometry;
    orcaice::setInit( geometry->offset );
    geometry->offset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Offset", geometry->offset );

    // consider the special case of the sensor mounted level (pitch=0) but upside-down (roll=180)
    bool compensateRoll;
    if ( NEAR(geometry->offset.o.r,M_PI,0.001) && geometry->offset.o.p==0.0 ) {
        // the offset is appropriate, now check the user preference (default is TRUE)
        compensateRoll = orcaice::getPropertyAsIntWithDefault( prop, prefix+"AllowRollCompensation", 1 );
    }
    else {
        // no need to consider it, the offset is inappropriate
        compensateRoll = false;
    }
    
    if ( compensateRoll ) {
        // now remove the roll angle, we'll compensate for it internally
        geometry->offset.o.r = 0.0;                            
        tracer()->info( "will compensate for upside-down mounted sensor" );
    }

    orcaice::setInit( geometry->size );
    geometry->size = orcaice::getPropertyAsSize3dWithDefault( prop, prefix+"Size", geometry->size );

    // EXTERNAL PROVIDED INTERFACE: Laser

    // create servant for direct connections
    // need the derived pointer to call custom functions
    LaserScanner2dI *laserScanner2dI = new LaserScanner2dI( geometry, "Laser", context() );
    laserObj_ = laserScanner2dI;
    orcaice::createInterfaceWithTag( context(), laserObj_, "Laser" );


    bool startEnabled = orcaice::getPropertyAsIntWithDefault( prop, prefix+"StartEnabled", true );

    orca::RangeScanner2dConfigPtr   laserConfig = new RangeScanner2dConfig;
    laserConfig->timeStamp = orcaice::getNow();

    double rangeResolution   = 
        (float)(orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"SickCarmen.RangeResolution", 0.01 ));

    laserConfig->maxRange = sicklaserutil::associatedMaxRange( rangeResolution );

    double angleIncrement = 
        (float)(orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"SickCarmen.AngularResolution", 1.0 )*DEG2RAD_RATIO);
    
    // This is the initial config if someone reads before we can enable
    laserConfig->isEnabled         = false;
    laserScanner2dI->localSetCurrentConfig( laserConfig );

    // This is what we'll immediately try to configure
    laserConfig->isEnabled         = startEnabled;

    // First check that the initial config is OK
    if ( !sicklaserutil::sickCanDoConfig( rangeResolution, angleIncrement, laserConfig->maxRange ) )
    {
        context().tracer()->warning( "Initial configuration could not be implemented!  Check config file." );
        laserConfig->isEnabled = false;
    }
    // Then ask the laser to set this config when we start up
    laserScanner2dI->localSetDesiredConfig( laserConfig );

    std::string driverName;
    int ret = orcaice::getProperty( prop, prefix+"Driver", driverName );

    if ( ret != 0 )
    {
        std::string errString = "Couldn't determine laser type.  Expected property '";
        errString += prefix + "Driver'";
        throw orcaice::Exception( ERROR_INFO, errString );
    }

    //
    // HARDWARE INTERFACES
    //
    
    if ( driverName == "sickcarmen" )
    {
#ifdef HAVE_CARMEN_DRIVER        
        const int baudrate = 
            orcaice::getPropertyAsIntWithDefault( prop, prefix+"SickCarmen.Baudrate", 38400 );

        std::string device    = orcaice::getPropertyWithDefault(      prop, prefix+"SickCarmen.Device",    "/dev/ttyS0" );
        std::string laserType = orcaice::getPropertyWithDefault(      prop, prefix+"SickCarmen.LaserType", "LMS" );
        hwDriver_ = new SickCarmenDriver( device.c_str(), laserType.c_str(), baudrate, context() );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driverName 'sickcarmen' because it wasn't built!" );
#endif
    }
    else if ( driverName == "playerclient" )
    {
#ifdef HAVE_PLAYERCLIENT_DRIVER        
        std::string playerHost = orcaice::getPropertyWithDefault(      prop, prefix+"PlayerClient.Host", "localhost" );
        int         playerPort = orcaice::getPropertyAsIntWithDefault( prop, prefix+"PlayerClient.Port", 6665 );
        int         playerDevice = orcaice::getPropertyAsIntWithDefault( prop, prefix+"PlayerClient.Device", 0 );
        std::string playerDriver = orcaice::getPropertyWithDefault( prop, prefix+"PlayerClient.Driver", "sicklms200" );
        hwDriver_ = new PlayerClientDriver( playerHost.c_str(), playerPort, playerDevice, playerDriver );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'playerclient' without player install!" );
#endif
    }
    else if ( driverName == "fake" )
    {
        hwDriver_ = new FakeDriver;
    }
    else
    {
        std::string errString = "unknown laser type: "+driverName;
        context().tracer()->error( errString );
        throw orcaice::Exception( ERROR_INFO, errString );
        return;
    }
    tracer()->debug( "loaded '"+driverName+"' driver", 2 );

    //
    // MAIN DRIVER LOOP
    //

    mainLoop_ = new MainLoop( *laserScanner2dI,
                               hwDriver_,
                               context(),
                               startEnabled,
                               compensateRoll );
    
    mainLoop_->start();    
}

void Component::stop()
{
    tracer()->info("stopping component...");
    orcaice::Thread::stopAndJoin( mainLoop_ );
}

}
