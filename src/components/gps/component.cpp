/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "component.h"
#include "mainloop.h"

#include "driver.h"
#include "fakegpsdriver.h"
#include "ashtech/ashtechdriver.h"

using namespace std;
using namespace orca;

namespace gps {

Component::Component()
    : orcaice::Component( "Gps" ),
      hwDriver_(0)
{
}

Component::~Component()
{
    delete hwDriver_;
    // do not delete the mainLoop_, it's a Thread and self-distructs.
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

    //
    // HARDWARE INTERFACES   
    //
    
    std::string driverName;
    int ret = orcaice::getProperty( prop, prefix+"Driver", driverName );
    if ( ret != 0 )
    {
        std::string errString = "Component: Couldn't determine gps type. Expected property '";
        errString += prefix + "Driver'";
        throw GpsException( errString );
    }

    if ( driverName == "ashtech" )
    {
        std::string device;
        int ret = orcaice::getProperty( prop, prefix+"Device", device );
        if (ret!=0) {
            std::string errString = "Component: Couldn't determine device (e.g. /dev/ttyS0) for ashtech. Expected property '";
            errString += prefix + "Device'";
            throw GpsException( errString );
        }
        hwDriver_ = new AshtechGpsDriver( device.c_str(), context() );
    }
    else if ( driverName == "fake" )
    {
        std::vector<double> latitudes, longitudes;
        for(unsigned int i=0; ; i++)
        {
            stringstream ss; ss << i;
            double latitude, longitude;
            if ( (orcaice::getPropertyAsDouble( prop, prefix+"FakeDriver.Latitude" + ss.str(), latitude )) ||
                 (orcaice::getPropertyAsDouble( prop, prefix+"FakeDriver.Longitude" + ss.str(), longitude )) )
            break;
            latitudes.push_back(latitude);
            longitudes.push_back(longitude);
        }
        hwDriver_ = new FakeGpsDriver(latitudes,longitudes);
    }
    else
    {
        std::string errString = "Component: Unknown driver: " + driverName;
        context().tracer()->error( errString );
        throw GpsException( errString );
    }

    // initialize driver
    try {
        hwDriver_->init();
    }
    catch( GpsException &e )
    {
        stringstream ss;
        ss << "Component: Failed to initialize GPS: " << e.what();
        context().tracer()->error( ss.str() );
        throw GpsException( ss.str() );
    }

    //
    // SENSOR DESCRIPTION   
    //
    
    descr_.timeStamp = orcaice::getNow();

    orcaice::setInit( descr_.offset );
    descr_.offset = orcaice::getPropertyAsFrame2dWithDefault( prop, prefix+"Offset", descr_.offset );

    orcaice::setInit( descr_.antennaOffset );
    descr_.antennaOffset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"AntennaOffset", descr_.antennaOffset );

    stringstream ss;
    ss << "Description: " << orcaice::toString( descr_ );
    context().tracer()->debug( ss.str() );

    // create servant for direct connections
    gpsInterface_        = new GpsImpl(        "Gps", descr_, context() );
    gpsMapGridInterface_ = new GpsMapGridImpl( "GpsMapGrid", descr_, context() );
    gpsTimeInterface_    = new GpsTimeImpl(    "GpsTime", descr_, context() );

    gpsInterface_->initInterface();
    gpsMapGridInterface_->initInterface();
    gpsTimeInterface_->initInterface();

    ////////////////////////////////////////////////////////////////////////////////

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    //
    // MAIN DRIVER LOOP
    //
    context().tracer()->debug( "entering mainLoop_...",5 );

    mainLoop_ = new MainLoop( gpsInterface_,
                              gpsMapGridInterface_,
                              gpsTimeInterface_,
                              hwDriver_,
                              descr_.antennaOffset,
                              context() );

    mainLoop_->start();
}

void Component::stop()
{
    tracer()->debug("stopping component...",2);
    orcaice::stopAndJoin( mainLoop_ );
    tracer()->debug("component stopped.",2);
}

}
