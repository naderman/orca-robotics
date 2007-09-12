/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Duncan Mercer, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <orcaice/orcaice.h>
#include "garmindriver.h"

using namespace std;
using namespace orca;
using namespace orcaserial;
using namespace orcagpsutil;

namespace gps {

GarminGpsDriver::GarminGpsDriver( const char             *device, 
                                    const orcaice::Context &context)
    : context_(context),
      serial_(device, 4800, true )
{
    // set timeouts for readLine: max one 1sec.
    serial_.setTimeout(1,0);
}

GarminGpsDriver::~GarminGpsDriver()
{
    disableDevice();
}

void
GarminGpsDriver::init()
{
    try {
        int numTries=0;
        while (resetDevice()!=0)
        {
            stringstream ss;
            ss << "Resetting device failed. Trying again (numTries: " << numTries << ")";
            context_.tracer()->warning(ss.str());
            if (numTries>2) {
                context_.tracer()->error("Giving up now...");
                throw GpsException("GarminGpsDriver: Resetting device failed twice.");
            }
            numTries++;
            sleep(1);
        }
        enableDevice();
    }
    catch ( const orcaserial::SerialException &e )
    {
        stringstream ss;
        ss << "GarminGpsDriver: Caught SerialException: " << e.what();
        context_.tracer()->error( ss.str() );
        throw GpsException( ss.str() );
    }
}

int
GarminGpsDriver::resetDevice()
{
    context_.tracer()->info("GarminGpsDriver: Trying to reset Garmin GPS device");

    serial_.flush();
    const char *reset_string = "$PASHS,RST\r\n";
    serial_.writeString(reset_string);
    char sdata[256];
    int ret;
    sleep(2);

    bool reset_ok=false;

    // do this multiple times just in case it is already spitting out data
    do
    {
        context_.tracer()->debug( "GarminGpsDriver: Trying to read a line from GPS" );

        ret = serial_.readLine(sdata,256,'\n');

        if(ret>0)
        {
            // stick a terminating char in to print
            if(strncmp(sdata,"$PASHR,ACK*3D",13)==0)
            {
                // AlexB: Why keep reading when we've reset OK ???
                reset_ok=true;
            }
            stringstream ssRet;
            ssRet << "Got " << ret << " bytes: " << sdata;
            context_.tracer()->debug( ssRet.str(), 2 );
        }
        else if ( ret < 0 )
        {
            context_.tracer()->debug( "GarminGpsDriver: read timed out..." );
        }
        else
        {
            context_.tracer()->error( "GarminGpsDriver: shouldn't get ret==0" );
        }
    }
    while(ret>0);

    if(reset_ok==false)
    {
        context_.tracer()->error("Resetting Garmin GPS device failed");   
        return -1;
    }

    // but of course we still have to wait a little longer grrrr....
    sleep(2);

    context_.tracer()->info("AshTech GPS reset sucessful");
    return 0;
}

void
GarminGpsDriver::enableDevice()
{
    context_.tracer()->info("Enabling Garmin GPS device");

    const char *gga_string = "$PASHS,NME,GGA,A,ON,1\r\n";
    const char *vtg_string = "$PASHS,NME,VTG,A,ON,1\r\n";
    const char *zda_string = "$PASHS,NME,ZDA,A,ON,1\r\n";
    const char *rre_string = "$PASHS,NME,RRE,A,ON,5\r\n";
    //const char *pos_string = "$PASHS,NME,POS,A,ON,1\r\n";
    //const char *utm_string = "$PASHS,NME,UTM,A,ON,1\r\n";

    serial_.writeString(gga_string);
    serial_.writeString(zda_string);
    serial_.writeString(vtg_string);
    serial_.writeString(rre_string);
    //ashtech specific
    //serial_.writeString(pos_string);
    //serial_.writeString(utm_string);
}

void
GarminGpsDriver::disableDevice()
{
    context_.tracer()->info("Disabling Garmin GPS device");
    
    const char *disable_string = "$PASHS,NME,ALL,A,OFF\r\n";
    serial_.writeString(disable_string);
    //force data to be sent
    serial_.drain();
}

int 
GarminGpsDriver::getData(orca::GpsData& data )
{
    if (newGpsData_) 
    {
        newGpsData_=false;
        data=GpsData_;
        return 0;
    }
    
    return -1;
}

int 
GarminGpsDriver::getTimeData(orca::GpsTimeData& data )
{
    if (newGpsTime_) 
    {
        newGpsTime_=false;
        data=GpsTimeData_;
        return 0;
    }
    
    return -1;
}

void
GarminGpsDriver::read()
{
    char serial_data[1024];

    // This will block up to the timeout
    int ret = serial_.readLine(serial_data,1024,'\n');
    timeOfRead_ = IceUtil::Time::now();
        
    if ( ret<0 ) {
        throw GpsException( "GarminGpsDriver: Timeout reading from serial port" );
    }
    
    if (ret==0) {
        throw GpsException("GarminGpsDriver: Read 0 bytes from serial port");
    }
    
    // 
    // We successfully read something from the serial port
    //

    nmeaMessage_.setSentence(serial_data);
    int check = nmeaParser_.parseMessage(nmeaMessage_);
    if (check!=0) {
        throw GpsException("GarminGpsDriver: Parsing message failed");
    }

    //printf("checksum is: 0x%x calc: 0x%x\n", nmeaMessage_.checksum(), nmeaMessage_.calcChecksum());
    if(nmeaMessage_.haveChecksum()) 
    {
        if(nmeaMessage_.checksum() != nmeaMessage_.calcChecksum()) {
            throw GpsException("GarminGpsDriver: Checksum is wrong");
        }
    }
    
    check = nmeaParser_.parseData(nmeaMessage_);
    if (check!=0) {
        throw GpsException("GarminGpsDriver: Parsing data failed");
    }
    
    // this may throw
    populateData();
}


void
GarminGpsDriver::populateData()
{
    //printf("got command %s\n",message.command());
    if(strncmp(nmeaMessage_.command(),"GPGGA",3)==0) {
        //cout << "got GGA message\n";
    
        //position type
        int positionType = atoi(nmeaMessage_.dataField(5)); 
        switch (positionType)
        {
            case 0: GpsData_.positionType = orca::GpsPositionTypeNotAvailable;
                    hasFix_ = false;
                    return;
            case 1: GpsData_.positionType = orca::GpsPositionTypeAutonomous;
                    break;
            case 2: GpsData_.positionType = orca::GpsPositionTypeDifferential;
                    break;
        }
    
        GpsData_.timeStamp = orcaice::toOrcaTime (timeOfRead_);
    
        //UTC time
        sscanf(nmeaMessage_.dataField(0),"%02d%02d%lf",
        &GpsData_.utcTime.hours, &GpsData_.utcTime.minutes, &GpsData_.utcTime.seconds);
        //position
        int deg;
        double min;
        double dir;
        //latitude
        sscanf(nmeaMessage_.dataField(1),"%02d%lf",&deg,&min);
        dir = (*nmeaMessage_.dataField(2)=='N') ? 1.0 : -1.0;
        GpsData_.latitude=dir*(deg+min/60.0);
        //longitude
        sscanf(nmeaMessage_.dataField(3),"%03d%lf",&deg,&min);
        dir = (*nmeaMessage_.dataField(4)=='E') ? 1.0 : -1.0;
        GpsData_.longitude=dir*(deg+min/60.0);
    
        //number of satellites
        GpsData_.satellites=atoi(nmeaMessage_.dataField(6));
    
        //altitude
        GpsData_.altitude=atof(nmeaMessage_.dataField(8));
    
        //geoidal Separation
        GpsData_.geoidalSeparation=atof(nmeaMessage_.dataField(10));
    
        // Set flag
        newGpsData_=true;
        hasFix_=true;
    
        return;

    } else if(strncmp(nmeaMessage_.command(),"GPZDA",3)==0) {

        GpsTimeData_.timeStamp = orcaice::toOrcaTime (timeOfRead_);

        //UTC time
        // should we check for an empy message ?
        int hours, minutes;
        double seconds;
        sscanf(nmeaMessage_.dataField(0),"%02d%02d%lf", &hours, &minutes, &seconds);
        
        GpsData_.utcTime.hours = hours;
        GpsData_.utcTime.minutes = minutes;
        GpsData_.utcTime.seconds = seconds;
        
        GpsTimeData_.utcTime.hours = hours;
        GpsTimeData_.utcTime.minutes = minutes;
        GpsTimeData_.utcTime.seconds = seconds;
        GpsTimeData_.utcDate.day=atoi(nmeaMessage_.dataField(1));
        GpsTimeData_.utcDate.month=atoi(nmeaMessage_.dataField(2));
        GpsTimeData_.utcDate.year=atoi(nmeaMessage_.dataField(3));
        newGpsTime_=true;
        return;

    } else if(strncmp(nmeaMessage_.command(),"GPVTG",3)==0) {
        //cout << "got VTG message\n";
        //check for empty message
        if(*nmeaMessage_.dataField(0)=='\0') {
            //cout << "VTK: empty msg" << endl;
            return;
        }
        //heading
        double headingRad = DEG2RAD(atof(nmeaMessage_.dataField(0)));
        NORMALISE_ANGLE( headingRad );
        GpsData_.heading=headingRad;
        //speed - converted to m/s
        GpsData_.speed=atof(nmeaMessage_.dataField(6));
        GpsData_.speed*=(1000/3600.0);
        //set to zero
        GpsData_.climbRate=0.0;
        newGpsData_=true;
        return;
        
    } else if(strncmp(nmeaMessage_.command(),"GPRRE",3)==0) {
	   //cout << "got RRE message\n";
       int numSatellites = atoi(nmeaMessage_.dataField(0));
       double rmsHorizontal = atof(nmeaMessage_.dataField(numSatellites*2+1));
       double rmsVertical = atof(nmeaMessage_.dataField(numSatellites*2+2));
       //cout << "rms hor/ver: " << rmsHorizontal << " " << rmsVertical << endl;
       GpsData_.horizontalPositionError = rmsHorizontal;
       GpsData_.verticalPositionError = rmsVertical;
       newGpsData_=true;
       return;
       
    } else if(strncmp(nmeaMessage_.command(),"PASHR",3)==0) {
        // Garmin specific messages
        //cout << "got PASHR message\n";
        if(strncmp(nmeaMessage_.dataField(0),"ACK",3)==0){
	       //cout << "got ACK response\n";
            return;
        } else if(strncmp(nmeaMessage_.dataField(0),"POS",3)==0) {
            //cout << "tk: got POS response\n";
	        //position type
            char *endptr;
            int ret=strtol(nmeaMessage_.dataField(1), &endptr, 10);
            //should be exactly one digit
            if(endptr!=nmeaMessage_.dataField(1)){
                // make like GGA message
                if (ret==0) {
                    GpsData_.positionType=orca::GpsPositionTypeAutonomous;
                } else if (ret==1) {
                    GpsData_.positionType=orca::GpsPositionTypeDifferential;
                }
            } else {
                GpsData_.positionType=orca::GpsPositionTypeNotAvailable;
                hasFix_=false;
                //cout << "Position not yet available\n";
                return;
            }

            GpsData_.timeStamp = orcaice::toOrcaTime (timeOfRead_);

            //number of satellites
            GpsData_.satellites=atoi(nmeaMessage_.dataField(2));

            //UTC time
            sscanf(nmeaMessage_.dataField(3),"%02d%02d%lf",
                &GpsData_.utcTime.hours, &GpsData_.utcTime.minutes, &GpsData_.utcTime.seconds);

            int deg;
            double min;
            double dir;
            //latitude
            sscanf(nmeaMessage_.dataField(4),"%02d%lf",&deg,&min);
            dir = (*nmeaMessage_.dataField(5)=='N') ? 1.0 : -1.0;
            GpsData_.latitude=dir*(deg+min/60.0);
            //longitude
            sscanf(nmeaMessage_.dataField(6),"%03d%lf",&deg,&min);
            dir = (*nmeaMessage_.dataField(7)=='E') ? 1.0 : -1.0;
            GpsData_.longitude=dir*(deg+min/60.0);

            //altitude
            GpsData_.altitude=atof(nmeaMessage_.dataField(8));
            //heading
            double headingRad = DEG2RAD(atof(nmeaMessage_.dataField(10)));
            NORMALISE_ANGLE( headingRad );
            GpsData_.heading=headingRad;
            //speed - convert from km/hr to m/s
            GpsData_.speed=atof(nmeaMessage_.dataField(11));
            GpsData_.speed*=(1000/3600.0);
            //climbrate
            GpsData_.climbRate=atof(nmeaMessage_.dataField(12));
            hasFix_=true;
            newGpsData_=true;
            return;
            
	   } else if(strncmp(nmeaMessage_.dataField(0),"UTM",3)==0){
            //cout << "got UTM response\n";
            int zone;
            char hem;
            sscanf(nmeaMessage_.dataField(2),"%02d%c",&zone,&hem);
            double easting;
            easting=atof(nmeaMessage_.dataField(3));
            double northing;
            northing=atof(nmeaMessage_.dataField(4));
            return;
	   }
 
    }
    
    // if we get here the msg is unknown
    throw GpsException("GarminGpsDriver: Unknown nmea message");
    
}

}
