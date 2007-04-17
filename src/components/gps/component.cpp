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

// Various bits of hardware we can drive
#include "driver.h"
#include "fakegpsdriver.h"
#include "ashtech/ashtechdriver.h"

// implementations of Ice objects
#include "gpsI.h"

using namespace std;
using namespace orca;

namespace gps {

Component::Component()
    : orcaice::Component( "Gps" ),
      mainLoop_(0),
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

    bool startEnabled = orcaice::getPropertyAsIntWithDefault( prop, prefix+"StartEnabled", true );

    //
    // HARDWARE INTERFACES   
    //
    
    std::string driverName;
    int ret = orcaice::getProperty( prop, prefix+"Driver", driverName );
    if ( ret != 0 )
    {
        std::string errString = "Couldn't determine gps type.  Expected property '";
        errString += prefix + "Driver'";
        throw orcaice::Exception( ERROR_INFO, errString );
    }

    if ( driverName == "ashtech" )
    {
        std::string device = orcaice::getPropertyWithDefault( prop, prefix+"Device", "/dev/ttyS0" );
        int baud = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Baud", 4800 );
        hwDriver_ = new AshtechGpsDriver( device.c_str(), baud);
    }
    else if ( driverName == "fake" )
    {
        std::vector<double> latitudes, longitudes;
        for(unsigned int i=0; ; i++)
        {
            stringstream* ss = new stringstream;
            *ss << i;
            double latitude, longitude;
            if ( (orcaice::getPropertyAsDouble( prop, prefix+"FakeDriver.Latitude" + ss->str(), latitude )) ||
                 (orcaice::getPropertyAsDouble( prop, prefix+"FakeDriver.Longitude" + ss->str(), longitude )) )
            break;
            latitudes.push_back(latitude);
            longitudes.push_back(longitude);
        }
        hwDriver_ = new FakeGpsDriver(latitudes,longitudes);
    }
    else
    {
        std::string errString = "unknown gps type: "+driverName;
        context().tracer()->error( errString );
        throw orcaice::Exception( ERROR_INFO, errString );
        return;
    }

    if(hwDriver_->reset()<0){
        std::string errString = "Failed to reset GPS.";
        context().tracer()->error( errString );
        throw orcaice::Exception( ERROR_INFO, errString );
        return;
    }
    
    hwDriver_->enable();

    //
    // SENSOR DESCRIPTION   
    //
    
    descr_.timeStamp = orcaice::getNow();

    orcaice::setInit( descr_.offset );
    descr_.offset = orcaice::getPropertyAsFrame2dWithDefault( prop, prefix+"Offset", descr_.offset );

    orcaice::setInit( descr_.antennaOffset );
    descr_.antennaOffset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"AntennaOffset", descr_.antennaOffset );

    cout << orcaice::toString( descr_ ) << endl;

    // hwDriver_->enable();

    // wait until we have a fix before publishing etc.
    /*
    do{
        ret=hwDriver_->read();
        if(ret==-1){
            std::string errString = "Failed to read from GPS.";
            context().tracer()->error( errString );
            throw orcaice::Exception( ERROR_INFO, errString );
            return;
        }
        if(!isActive()){
            return;
        }
    }while(!hwDriver_->hasFix());
    */

    // create servant for direct connections
    GpsI* gpsObj_ = new GpsI(descr_, context());

    gpsObjPtr_ = gpsObj_;

    orcaice::createInterfaceWithTag( context(), gpsObjPtr_, "Gps" );

    ////////////////////////////////////////////////////////////////////////////////

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    //
    // MAIN DRIVER LOOP
    //
    context().tracer()->debug( "entering mainLoop_...",5 );

    mainLoop_ = new MainLoop(*gpsObj_,
                              hwDriver_,
                              context(),
                              startEnabled );

    mainLoop_->start();
}

void Component::stop()
{
    // Tell the main loop to stop
    if(mainLoop_!=NULL){
        mainLoop_->stop();

        IceUtil::ThreadControl tc = mainLoop_->getThreadControl();

        // Then wait for it
        tc.join();

        // When the ThreadControl object goes out of scope thread is also deleted
        // how dumb ! in this case Thread is mainLoop_...
    }
}

}
