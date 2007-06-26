/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "component.h"

#include "sickutil/sickutil.h"

namespace laser2d {

using namespace std;
using namespace orca;

Component::Component()
    : orcaice::Component( "Laser2d" )
{
}

Component::~Component()
{
    // do not delete mainLoop_!!! It is held in a smart pointer and will self-destruct.
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
    // SENSOR DESCRIPTION
    //

    orca::RangeScanner2dDescription descr;
    descr.timeStamp = orcaice::getNow();

    // transfer internal sensor configs
    descr.minRange        = cfg.minRange;
    descr.maxRange        = cfg.maxRange;
    descr.fieldOfView     = cfg.fieldOfView;
    descr.startAngle      = cfg.startAngle;
    descr.numberOfSamples = cfg.numberOfSamples;

    // offset from the robot coordinate system
    orcaice::setInit( descr.offset );
    descr.offset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Offset", descr.offset );

    // consider the special case of the sensor mounted level (pitch=0) but upside-down (roll=180)
    bool compensateRoll;
    if ( NEAR(descr.offset.o.r,M_PI,0.001) && descr.offset.o.p==0.0 ) {
        // the offset is appropriate, now check the user preference (default is TRUE)
        compensateRoll = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"AllowRollCompensation", 1 );

        if ( compensateRoll ) {
            // now remove the roll angle, we'll compensate for it internally
            descr.offset.o.r = 0.0;
            tracer()->info( "the driver will compensate for upside-down mounted sensor" );
        }
    }
    else {
        // no need to consider it, the offset is inappropriate for roll compensation
        compensateRoll = false;
    }

    // size info should really be stored in the driver
    orcaice::setInit( descr.size );
    descr.size = orcaice::getPropertyAsSize3dWithDefault( prop, prefix+"Size", descr.size );

    //
    // EXTERNAL PROVIDED INTERFACE
    //

    laserInterface_ = new orcaifaceimpl::LaserScanner2dIface( descr,
                                                              "LaserScanner2d",
                                                              context() );

    //
    // MAIN DRIVER LOOP
    //

    mainLoop_ = new MainLoop( *laserInterface_,
                               cfg,
                               compensateRoll,
                               context() );
    
    mainLoop_->start();
}

void 
Component::stop()
{
    tracer()->debug("stopping component...",2);
    orcaice::stopAndJoin( mainLoop_ );
    tracer()->debug("component stopped.",2);
}

} // namespace
