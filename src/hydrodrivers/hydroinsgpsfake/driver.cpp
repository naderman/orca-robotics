/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <IceUtil/IceUtil.h>
#include <stdlib.h>

#include "driver.h"

using namespace std;

using namespace insgpsfake;
namespace hif = hydrointerfaces;

namespace{
    std::auto_ptr<hif::InsGps::GenericData>
    generateFakeIns();
    std::auto_ptr<hif::InsGps::GenericData>
    generateFakeGps();
    std::auto_ptr<hif::InsGps::GenericData>
    generateFakeImu();
}
Driver::Driver( const Config &cfg, const hydroutil::Context &context ) :
    config_(cfg),
    context_(context)
{
}

std::auto_ptr<hif::InsGps::GenericData>
Driver::read()
{
    static int t;
    static unsigned int randSeed;

    while(1){
        // you are very welcome to do this in a more elegant way
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(1));
        t++;

        if(0 == t%10){
            //imu every 10ms
            static int cnt;
            if(0 == (int) (101.0 * (rand_r(&randSeed) / (RAND_MAX + 1.0)))){
                std::ostringstream ss;
                ss << "Generating fake imu data... " << cnt << " so far" << std::ends;
                context_.tracer().info( ss.str() );
            }
            cnt++;
            return generateFakeImu();
        }
        if(0 == (t+1)%20){
            //ins every 20ms
            static int cnt;
            if(2 > (int) (101.0 * (rand_r(&randSeed) / (RAND_MAX + 1.0)))){
                std::ostringstream ss;
                ss << "Generating fake ins data... " << cnt << " so far" << std::ends;
                context_.tracer().info( ss.str() );
            }
            cnt++;
            return generateFakeIns();
        }
        if(0 == (t+2)%200){
            //gps every 200ms
            static int cnt;
            if(20 > (int) (101.0 * (rand_r(&randSeed) / (RAND_MAX + 1.0)))){
                std::ostringstream ss;
                ss << "Generating fake gps data... " << cnt << " so far" << std::ends;
                context_.tracer().info( ss.str() );
            }
            cnt++;
            return generateFakeGps();
        }
    }
}

namespace{

    void fillInFakeStatus(hif::InsGps::GenericData &data){
        static unsigned int randSeed;
        switch( (int) (1001.0 * (rand_r(&randSeed) / (RAND_MAX + 1.0)))){
            case 1:
                // 1/1000 change to report Ok
                data.statusMessageType = hif::InsGps::Ok;
                data.statusMessage = "Just feeling chatty";
                break;
            case 2:
                // 1/1000 change to report Warning
                data.statusMessageType = hif::InsGps::Warning;
                data.statusMessage = "Feeling uneasy";
                break;
            case 3:
                // 1/1000 change to report Fault
                data.statusMessageType = hif::InsGps::Fault;
                data.statusMessage = "About to burst into flames";
                break;
            default:
                // 997/1000 change to report nothing
                data.statusMessageType = hif::InsGps::NoMsg;
                data.statusMessage = "";
                break;
        }
        return;
    }
    std::auto_ptr<hif::InsGps::GenericData> generateFakeIns(){
        static int i;
        hif::InsGps::InsData *data = new hif::InsGps::InsData;
        std::auto_ptr<hif::InsGps::GenericData> genericData( data );
        data->lat = (i++)%100;
        data->lon = (i++)%100;
        data->alt = (i++)%100;
        data->altAMSL = (i++)%100;
        data->vENU[0] = (i++)%100;
        data->vENU[1] = (i++)%100;
        data->vENU[2] = (i++)%100;
        data->oRPY[0] = (i++)%100;
        data->oRPY[1] = (i++)%100;
        data->oRPY[2] = (i++)%100;

        IceUtil::Time t = IceUtil::Time::now();
        data->time.tv_sec = (int)t.toSeconds();
        data->time.tv_usec = (int)t.toMicroSeconds() - data->time.tv_sec*1000000;
        fillInFakeStatus(*(genericData.get()));

        return genericData;
    }

    std::auto_ptr<hif::InsGps::GenericData> generateFakeGps(){
        static int i;
        hif::InsGps::GpsData *data = new hif::InsGps::GpsData;
        std::auto_ptr<hif::InsGps::GenericData> genericData( data );

        data->lat = (i++)%100;
        data->lon = (i++)%100;
        data->alt = (i++)%100;
        data->altAMSL = (i++)%100;
        data->sigmaLat = (i++)%100;
        data->sigmaLon = (i++)%100;
        data->sigmaAlt = (i++)%100;
        data->heading = (i++)%100;
        data->speed = (i++)%100;
        data->climbRate = (i++)%100;
        data->sat = (i++)%100;
        data->obsL1 = (i++)%100;
        data->obsL2 = (i++)%100;

        IceUtil::Time t = IceUtil::Time::now();
        data->time.tv_sec = (int)t.toSeconds();
        data->time.tv_usec = (int)t.toMicroSeconds() - data->time.tv_sec*1000000;
        fillInFakeStatus(*(genericData.get()));

        return genericData;
    }

    std::auto_ptr<hif::InsGps::GenericData> generateFakeImu(){
        static int i;
        hif::InsGps::ImuData *data = new hif::InsGps::ImuData;
        std::auto_ptr<hif::InsGps::GenericData> genericData( data );

        data->acc[0] = (i++)%100;
        data->acc[1] = (i++)%100;
        data->acc[2] = (i++)%100;
        data->turnRate[0] = (i++)%100;
        data->turnRate[1] = (i++)%100;
        data->turnRate[2] = (i++)%100;
        data->biasCorrected = (i++)%100;

        IceUtil::Time t = IceUtil::Time::now();
        data->time.tv_sec = (int)t.toSeconds();
        data->time.tv_usec = (int)t.toMicroSeconds() - data->time.tv_sec*1000000;
        fillInFakeStatus(*(genericData.get()));

        return genericData;
    }
}

extern "C" {
    hif::InsGpsFactory *createInsGpsDriverFactory()
    { return new insgpsfake::Factory; }
}
