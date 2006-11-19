/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "component.h"
#include "mainloop.h"

// implementations of Ice objects
#include "laserscanner2dI.h"

// Various bits of hardware we can drive
#include "driver.h"
#include "fakedriver.h"
#ifdef HAVE_CARMEN_DRIVER
#  include "sickcarmen/sickcarmendriver.h"
#endif
#ifdef HAVE_PLAYERCLIENT_DRIVER
#  include "playerclient/playerclientdriver.h"
#endif
#include "sickutil/sickutil.h"

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
    
    // do not delete mainLoop_!!! It derives from Ice::Thread and self-destructs.
}

void
Component::start()
{
    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag()+".Config.";

    //
    // DRIVER CONFIGURATION
    //

    // read config options
    Driver::Config cfg;

    cfg.minRange = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinRange", 0.0 );
    cfg.maxRange = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxRange", 80.0 );

    cfg.fieldOfView = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"FieldOfView", 180.0 )*DEG2RAD_RATIO;
    cfg.startAngle = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"StartAngle", -RAD2DEG(cfg.fieldOfView)/2.0 )*DEG2RAD_RATIO;

    cfg.numberOfSamples = orcaice::getPropertyAsIntWithDefault( prop, prefix+"NumberOfSamples", 181 );

    if ( !cfg.validate() ) {
        tracer()->error( "Failed to validate laser configuration. "+cfg.toString() );
        // this will kill this component
        throw orcaice::Exception( ERROR_INFO, "Failed to validate laser configuration" );
    }

    //
    // HARDWARE INTERFACES
    //
    std::string driverName = orcaice::getPropertyWithDefault( prop, prefix+"Driver", "sickcarmen" );

    if ( driverName == "sickcarmen" )
    {
#ifdef HAVE_CARMEN_DRIVER
        context().tracer()->debug( "loading 'sickcarmen' driver",3);
        hwDriver_ = new SickCarmenDriver( cfg, context() );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'sickcarmen' because it wasn't built!" );
#endif
    }
    else if ( driverName == "playerclient" )
    {
#ifdef HAVE_PLAYERCLIENT_DRIVER
        context().tracer()->debug( "loading 'playerclient' driver",3);
        hwDriver_ = new PlayerClientDriver( cfg, context() );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'playerclient' because it wasn't built!" );
#endif
    }
    else if ( driverName == "fake" )
    {
        context().tracer()->debug( "loading 'fake' driver",3);
        hwDriver_ = new FakeDriver( cfg, context() );
    }
    else
    {
        std::string errString = "Unknown laser type: "+driverName;
        context().tracer()->error( errString );
        throw orcaice::Exception( ERROR_INFO, errString );
        return;
    }
    tracer()->debug( "Loaded '"+driverName+"' driver", 2 );

    //
    // SENSOR DESCRIPTION
    //

    orca::RangeScanner2dDescriptionPtr descr = new orca::RangeScanner2dDescription;
    descr->timeStamp = orcaice::getNow();

    // transfer internal sensor configs
    descr->minRange        = cfg.minRange;
    descr->maxRange        = cfg.maxRange;
    descr->fieldOfView     = cfg.fieldOfView;
    descr->startAngle      = cfg.startAngle;
    descr->numberOfSamples = cfg.numberOfSamples;

    // offset from the robot coordinate system
    orcaice::setInit( descr->offset );
    descr->offset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Offset", descr->offset );

    // consider the special case of the sensor mounted level (pitch=0) but upside-down (roll=180)
    bool compensateRoll;
    if ( NEAR(descr->offset.o.r,M_PI,0.001) && descr->offset.o.p==0.0 ) {
        // the offset is appropriate, now check the user preference (default is TRUE)
        compensateRoll = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"AllowRollCompensation", 1 );

        if ( compensateRoll ) {
            // now remove the roll angle, we'll compensate for it internally
            descr->offset.o.r = 0.0;
            tracer()->info( "the driver will compensate for upside-down mounted sensor" );
        }
    }
    else {
        // no need to consider it, the offset is inappropriate for roll compensation
        compensateRoll = false;
    }

    // size info should really be stored in the driver
    orcaice::setInit( descr->size );
    descr->size = orcaice::getPropertyAsSize3dWithDefault( prop, prefix+"Size", descr->size );

    //
    // EXTERNAL PROVIDED INTERFACE: LaserScanner2d
    //

    // create servant for direct connections
    // need the derived pointer to call custom functions
    LaserScanner2dI *laserScanner2dI = new LaserScanner2dI( descr, "LaserScanner2d", context() );
    // to register with the adapter, it's enough to have a generic pointer
    laserObj_ = laserScanner2dI;
    // this may throw but it's better if it kills us.
    orcaice::createInterfaceWithTag( context(), laserObj_, "LaserScanner2d" );


    //
    // MAIN DRIVER LOOP
    //

    mainLoop_ = new MainLoop( *laserScanner2dI,
                               hwDriver_,
                               compensateRoll,
                               context() );
    
    mainLoop_->start();
}

void Component::stop()
{
    tracer()->info("stopping component...");
    orcaice::Thread::stopAndJoin( mainLoop_ );
}

} // namespace
