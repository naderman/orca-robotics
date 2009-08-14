/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Duncan Mercer, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <IceUtil/IceUtil.h>
#include <hydroutil/hydroutil.h>
#include "driver.h"

using namespace std;

namespace gpsgarmingbx {

Driver::Driver( const hydrointerfaces::Gps::Config &cfg, const hydroutil::Context &context ) :
    config_(cfg),
    context_(context)
{
    std::string prefix = "Garmin.";

    gbxgarminacfr::Config gbxConfig;

    gbxConfig.device = context_.properties().getPropertyWithDefault( prefix+"Device", "/dev/ttyS0" );
    gbxConfig.readGga = true;
    gbxConfig.readVtg = true;
    gbxConfig.readRme = true;
    gbxConfig.ignoreUnknown = true;

    device_.reset( new gbxgarminacfr::Driver( gbxConfig, context_.tracer(), context_.status() ) );
}


void 
Driver::read( hydrointerfaces::Gps::Data& data )
{      
    std::auto_ptr<gbxgarminacfr::GenericData> gbxData;

    //How many messages are we looking for to make our frame
    const int N_MSGS_IN_FRAME = 3;
    bool haveGga = false;
    bool haveVtg = false;
    bool haveRme = false;
    int msgCount = 0;

    // these are not reported by this driver
    data.climbRate = 0.0;
    data.observationCountOnL1 = 0;
    data.observationCountOnL2 = 0;

    while( !( haveGga && haveVtg && haveRme ) && !(msgCount > N_MSGS_IN_FRAME) )
    {
        // catch exceptions!!!
        try {
            gbxData = device_->read();
        }
        catch ( const std::exception& e ) {
            cout<<"hydro::Driver::read(): Failed to read GPS message: "<<e.what()<<endl;
            throw;
        }

        // find out which data type it is
        switch ( gbxData->type() )
        {
            case gbxgarminacfr::GpGga :
            {
//                 cout<<"GPGGA"<<endl;
                gbxgarminacfr::GgaData* d = (gbxgarminacfr::GgaData*)gbxData.get();
                data.timeStampSec = d->timeStampSec;
                data.timeStampUsec = d->timeStampUsec;
                data.utcTimeHrs = d->utcTimeHrs;
                data.utcTimeMin = d->utcTimeMin;
                data.utcTimeSec = d->utcTimeSec;
                switch ( d->fixType ) 
                {
                case gbxgarminacfr::Invalid : 
                    data.positionType = hydrointerfaces::Gps::GpsPositionTypeNotAvailable;
                    break;
                case gbxgarminacfr::Autonomous :  
                    data.positionType = hydrointerfaces::Gps::GpsPositionTypeAutonomous;
                    break;
                case gbxgarminacfr::Differential : 
                    data.positionType = hydrointerfaces::Gps::GpsPositionTypeDifferential;
                    break;
                }
                data.latitude = d->latitude;
                data.longitude = d->longitude;
                data.altitude = d->altitude;
                data.satellites = d->satellites;
                data.geoidalSeparation = d->geoidalSeparation;
                haveGga = true;
                break;
            }
            case gbxgarminacfr::GpVtg :
            {
                // wait for GPGGA message
                if ( !haveGga )
                    break;
//                 cout<<"GPVTG"<<endl;
                gbxgarminacfr::VtgData* d = (gbxgarminacfr::VtgData*)gbxData.get();
                data.speed = d->speed;
                data.heading = d->headingTrue;
                haveVtg = true;
                break;
            }
            case gbxgarminacfr::PgRme :
            {
                // wait for GPGGA message
                if ( !haveGga )
                    break;
//                 cout<<"PGRME"<<endl;
                gbxgarminacfr::RmeData* d = (gbxgarminacfr::RmeData*)gbxData.get();
                data.horizontalPositionError = d->horizontalPositionError;
                data.verticalPositionError = d->verticalPositionError;
                haveRme = true;
                break;
            }
            default :
                cout<<"?????"<<endl;
        }

        ++msgCount;
    }
}

} // namespace

extern "C" {
    hydrointerfaces::GpsFactory *createDriverFactory()
    { return new gpsgarmingbx::Factory; }
}
