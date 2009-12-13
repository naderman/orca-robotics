/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Duncan Mercer, Alex Brooks, Alexei Makarenko, Tobias Kaupp, Michael Warren
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <IceUtil/IceUtil.h>
#include <hydroutil/hydroutil.h>
#include "driver.h"
#include <gbxutilacfr/exceptions.h>

using namespace std;

namespace gpsgarmingbx {

namespace {

    void convertGga( const gbxgarminacfr::GgaData &ggaData,
                     hydrointerfaces::Gps::Data   &data )
    {
        data.timeStampSec  = ggaData.timeStampSec;
        data.timeStampUsec = ggaData.timeStampUsec;
        data.utcTimeHrs    = ggaData.utcTimeHrs;
        data.utcTimeMin    = ggaData.utcTimeMin;
        data.utcTimeSec    = ggaData.utcTimeSec;
        switch ( ggaData.fixType ) 
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
        data.latitude          = ggaData.latitude;
        data.longitude         = ggaData.longitude;
        if ( ggaData.isAltitudeKnown )
        {
            data.altitude          = ggaData.altitude;
        }
        else
        {
            // TODO: handle this better...
            data.altitude = -999999;
        }
        data.satellites        = ggaData.satellites;
        data.geoidalSeparation = ggaData.geoidalSeparation; 
    }

    void convert( const gbxgarminacfr::GgaData &ggaData,
                  const gbxgarminacfr::VtgData &vtgData,
                  const gbxgarminacfr::RmeData &rmeData,
                  hydrointerfaces::Gps::Data   &data )
    {
        if ( ( !vtgData.isValid ||
               !rmeData.isValid  ) &&
             ggaData.fixType != gbxgarminacfr::Invalid )
        {
            stringstream ss;
            ss << "Bad combination of packets, invalid vtg/rme with valid fix:" << endl
               << "ggaData: " << ggaData << endl
               << "vtgData: " << vtgData << endl
               << "rmeData: " << rmeData;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }

        // GGA
        convertGga(ggaData,data );

        // VTG
        data.speed   = vtgData.speed;
        data.heading = vtgData.headingTrue;

        // RME
        data.horizontalPositionError      = rmeData.horizontalPositionError;
        data.isVerticalPositionErrorValid = rmeData.isVerticalPositionErrorValid;
        data.verticalPositionError        = rmeData.verticalPositionError;      
    }

    void convert( const gbxgarminacfr::GgaData &ggaData,
                  const gbxgarminacfr::VtgData &vtgData,
                  hydrointerfaces::Gps::Data   &data )
    {
        if ( !vtgData.isValid && ggaData.fixType != gbxgarminacfr::Invalid )
        {
            stringstream ss;
            ss << "Bad combination of packets, invalid gga/vtg with valid fix:" << endl
               << "ggaData: " << ggaData << endl
               << "vtgData: " << vtgData <<endl;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    
        // GGA
        convertGga(ggaData,data );

        // VTG
        data.speed   = vtgData.speed;
        data.heading = vtgData.headingTrue;
    
    }

    void convert( const gbxgarminacfr::GgaData &ggaData,
                  const gbxgarminacfr::RmcData &rmcData,
                  hydrointerfaces::Gps::Data   &data )
    {
        if ( !rmcData.isValid && ggaData.fixType != gbxgarminacfr::Invalid )
        {
            stringstream ss;
            ss << "Bad combination of packets, invalid gga/rmc with valid fix:" << endl
               << "ggaData: " << ggaData << endl
               << "rmcData: " << rmcData <<endl;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
        
        // GGA
        convertGga(ggaData,data );

        // RMC
        data.speed   = rmcData.speed;
        data.heading = rmcData.headingTrue;
    
    }

}

Driver::Driver( const hydrointerfaces::Gps::Config &cfg, const hydroutil::Context &context ) :
    config_(cfg),
    context_(context)
{
    std::string prefix = "Garmin.";

    gbxgarminacfr::Config gbxConfig;

    gbxConfig.device = context_.properties().getPropertyWithDefault( prefix+"Device", "/dev/ttyS0" );
    protocol = context_.properties().getPropertyWithDefault( prefix+"Protocol", "Garmin" );

    if (protocol == "Garmin")
    {
        gbxConfig.readGga = true;
        gbxConfig.readVtg = true;
        gbxConfig.readRme = true;
        gbxConfig.readRmc = false;
        gbxConfig.ignoreUnknown = true;
    }
    else if (protocol == "NMEA")
    {
        gbxConfig.readGga = true;
        gbxConfig.readVtg = true;
        gbxConfig.readRme = false;
        gbxConfig.readRmc = true;
        gbxConfig.ignoreUnknown = true;
    }
    else
    {
        stringstream ss;
        ss << "hydro::Driver::Driver(): Unknown protocol \"" << 
            protocol << "\", expected \"Garmin\" or \"NMEA\"." << endl;
    }
    device_.reset( new gbxgarminacfr::Driver( gbxConfig, context_.tracer(), context_.status() ) );
}


void 
Driver::read( hydrointerfaces::Gps::Data& data )
{      
    std::auto_ptr<gbxgarminacfr::GenericData> gbxData;

    // If there's no complete hydrointerfaces::Gps::Data structure after this many messages, something is wrong.
    const int MAX_MESSAGES = 5;
    int msgCount = 0;

    std::auto_ptr<gbxgarminacfr::GgaData> ggaDataPtr;
    std::auto_ptr<gbxgarminacfr::VtgData> vtgDataPtr;
    std::auto_ptr<gbxgarminacfr::RmeData> rmeDataPtr;
    std::auto_ptr<gbxgarminacfr::RmcData> rmcDataPtr;

    // these are not reported by this driver
    data.climbRate = 0.0;
    data.observationCountOnL1 = 0;
    data.observationCountOnL2 = 0;

    if (protocol == "NMEA") {
        // Standard NMEA does not provide this data
        data.horizontalPositionError = 0;
        data.isVerticalPositionErrorValid = 0;
        data.verticalPositionError = 0;
    }

    // This evaluation is a bit ugly, may need to change in future
    while( !(protocol == "Garmin" && ggaDataPtr.get() && vtgDataPtr.get() && rmeDataPtr.get() ) 
            && !(protocol == "NMEA" && ggaDataPtr.get() && (vtgDataPtr.get() || rmcDataPtr.get() ) ) )
    {
        if ( ++msgCount > MAX_MESSAGES )
        {
            stringstream ss;
            const bool haveGga = ggaDataPtr.get();
            const bool haveVtg = vtgDataPtr.get();
            const bool haveRme = rmeDataPtr.get();
            const bool haveRmc = rmcDataPtr.get();
            ss << "Failed to receive all message types after " << msgCount << " msgs." << endl
               << "haveGga: " << haveGga << ", haveVtg: " << haveVtg << ", haveRme: " << haveRme
               << "haveRmc: " << haveRmc;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }

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
                ggaDataPtr.reset( (gbxgarminacfr::GgaData*)gbxData.release() );
                break;
            }
            case gbxgarminacfr::GpVtg:
            {
                // wait for GPGGA message
                if ( !ggaDataPtr.get() )
                    break;
                vtgDataPtr.reset( (gbxgarminacfr::VtgData*)gbxData.release() );
                break;
            }
            case gbxgarminacfr::PgRme:
            {
                // wait for GPGGA message
                if ( !ggaDataPtr.get() )
                    break;
                rmeDataPtr.reset( (gbxgarminacfr::RmeData*)gbxData.release() );
                break;
            }
            case gbxgarminacfr::GpRmc:
            {
                // wait for GPGGA message
                if ( !ggaDataPtr.get() )
                    break;
                rmcDataPtr.reset( (gbxgarminacfr::RmcData*)gbxData.release() );
                break;
            }
            default:
            {
                stringstream ss;
                ss << "Unknown message type: " << gbxData->type();
                cout << ss.str() << endl << "sending message upwards regardless (TODO: fix me)" << endl;
                //throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
                break;
            }
        }
    }

    // Now that we have the correct combination of messages, which is it?
    if (vtgDataPtr.get() && rmeDataPtr.get())
        // Garmin GGA + VTG + RME message combination
        convert( *ggaDataPtr, *vtgDataPtr, *rmeDataPtr, data);

    else if (vtgDataPtr.get())
        // NMEA GGA + VTG message combination
        convert( *ggaDataPtr, *vtgDataPtr, data);

    else if (rmcDataPtr.get())
        // NMEA GGA + RMC message combination
        convert( *ggaDataPtr, *rmcDataPtr, data );

    else {
        stringstream ss;
        ss << "Bad message combination: " <<  "haveGga: " << ggaDataPtr.get() << ", haveVtg: " << 
            vtgDataPtr.get() << ", haveRme: " << rmeDataPtr.get() << "haveRmc: " << rmcDataPtr.get();
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

} // namespace

extern "C" {
    hydrointerfaces::GpsFactory *createGpsDriverFactory()
    { return new gpsgarmingbx::Factory; }
}
