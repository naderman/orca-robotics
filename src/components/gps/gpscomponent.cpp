/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <orcaice/orcaice.h>

#include "gpscomponent.h"
#include "gpshandler.h"

// Various bits of hardware we can drive
#include "gpsdriver.h"
#include "fakegpsdriver.h"
#include "ashtech/ashtechdriver.h"

// implementations of Ice objects
#include "gpsI.h"

using namespace std;
using namespace orca;

GpsComponent::GpsComponent()
    : orcaice::Component( "Gps" ),
      handler_(NULL),
      config_(new GpsConfigData),
      hwDriver_(NULL)
{

}

GpsComponent::~GpsComponent()
{
    delete hwDriver_;
    // do not delete the handler_, it's a Thread and self-distructs.
}

void
GpsComponent::start()
{
    //
    // INITIAL CONFIGURATION
    //

    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag();
    prefix += ".Config.";

    bool startEnabled = orcaice::getPropertyAsIntWithDefault( prop, prefix+"StartEnabled", true );

    config_->geometry.x = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Geometry.x", 0.0 );
    config_->geometry.y = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Geometry.y", 0.0 );
    config_->geometry.z = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Geometry.z", 0.0 );

    //set to default
    orcaice::setInit( config_->origin );
    config_->origin = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Offset", config_->origin );

    std::string driverName;
    int ret = orcaice::getProperty( prop, prefix+"Driver", driverName );
    if ( ret != 0 )
    {
        std::string errString = "Couldn't determine gps type.  Expected property '";
        errString += prefix + "Driver'";
        throw orcaice::Exception( ERROR_INFO, errString );
    }

    //
    // HARDWARE INTERFACES
    //

    
    if ( driverName == "ashtech" )
    {
        std::string device = orcaice::getPropertyWithDefault( prop, prefix+"Device", "/dev/ttyS0" );
        int baud = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Baud", 4800 );

        hwDriver_ = new AshtechGpsDriver( device.c_str(), baud);
    }
    else if ( driverName == "fake" )
    {
        hwDriver_ = new FakeGpsDriver;
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
    GpsI* gpsObj_ = new GpsI(config_,context());

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
    context().tracer()->debug( "entering handler_...",5 );

    handler_ = new GpsHandler(*gpsObj_,
                              hwDriver_,
                              context(),
                              startEnabled );

    handler_->start();
}

void GpsComponent::stop()
{
    // Tell the main loop to stop
    if(handler_!=NULL){
        handler_->stop();

        IceUtil::ThreadControl tc = handler_->getThreadControl();

        // Then wait for it
        tc.join();

        // When the ThreadControl object goes out of scope thread is also deleted
        // how dumb ! in this case Thread is handler_...
    }
}
