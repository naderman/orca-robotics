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
#include <string>
#include <sstream>

#include <orcaice/orcaice.h>
#include "garmindriver.h"

using namespace std;
using namespace orca;
using namespace orcaserial;
using namespace orcagpsutil;
using namespace gps;

//***************************************************************************

GarminGpsDriver::GarminGpsDriver( const char             *device, 
                                    const orcaice::Context &context)
    : context_(context),
      serial_(device, 4800, true )
{
    // set timeouts for readLine: max one 1sec.
    serial_.setTimeout(1,0);
}

//*******************************************************************************

GarminGpsDriver::~GarminGpsDriver()
{
    disableDevice();
}


//********************************************************************************

void
GarminGpsDriver::init()
{
    try {
        enableDevice();
        //TODO Need to check here that we have been successful.
    }
    catch ( const orcaserial::SerialException &e )
    {
        stringstream ss;
        ss << "GarminGpsDriver: Caught SerialException: " << e.what();
        context_.tracer()->error( ss.str() );
        throw GpsException( ss.str() );
    }
}


//*****************************************************************************

void
GarminGpsDriver::enableDevice()
{

    //Create the messages that we are going to send and add the checksums
    //Note that the checksum field is filled with 'x's before we start
    NmeaMessage DisableAllMsg("$PGRMO,,2*xx\r\n",AddChecksum);
    NmeaMessage Start_GGA_Msg("$PGRMO,GPGGA,1*xx\r\n",AddChecksum);
    NmeaMessage Start_VTG_Msg("$PGRMO,GPVTG,1*xx\r\n",AddChecksum);
    NmeaMessage Start_RME_Msg("$PGRMO,PGRME,1*xx\r\n",AddChecksum);

    context_.tracer()->info("Configure Garmin GPS device");


    //First disables all output messages then enable selected ones only.
    serial_.writeString(DisableAllMsg.sentence());
    sleep(1);
    
    serial_.writeString(Start_GGA_Msg.sentence());
    serial_.writeString(Start_VTG_Msg.sentence());
    serial_.writeString(Start_RME_Msg.sentence());
    sleep(1);

}


//***********************************************************************
void
GarminGpsDriver::disableDevice()
{

    //Simply send the no messages command!
    NmeaMessage DisableAllMsg("$PGRMO,,2*xx\r\n",AddChecksum);
    serial_.writeString(DisableAllMsg.sentence());
}



//*******************************************************************************

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

//****************************************************************************

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


//****************************************************************************


void
GarminGpsDriver::read()
{
    char serial_data[1024];
    static int nmeaExceptionCount =0;

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
    
    //Put it into the message object and checksum the data
    try{
        //This throws if it cannot find the * to deliminate the checksum field
        nmeaMessage_.setSentence(serial_data,TestChecksum);
    }
    catch (NmeaException &e)
    {
        //Don't throw if only occasional messages are missing the checksums
        if(nmeaExceptionCount++ < 3) {return;}
        stringstream ss;
        ss << "MainLoop: Problem reading from GPS: " << e.what();
        context_.tracer()->error( ss.str() );
        throw GpsException(ss.str());
    }

    nmeaExceptionCount = 0;
    
    if(!nmeaMessage_.haveValidChecksum()){        
        throw GpsException("GarminGpsDriver: Message fails checksum");
    }

    // this may throw.
    populateData();
}


//**********************************************************************************

void
GarminGpsDriver::populateData()
{
    //First split up the data fields in the string we have read.
    nmeaMessage_.parseTokens();
    
    //And then find out which type of messge we have recieved...
    string MsgType(nmeaMessage_.getDataToken(0));

    if(nmeaMessage_.haveTestedChecksum() && (!nmeaMessage_.haveValidChecksum())){
        throw GpsException("GarminGpsDriver: Message fails checksum");
    }
    
    if(MsgType == "$GPGGA"){
        //cout << "got GGA message\n";
        ExtractGGAData();
        return;
    }else if(MsgType == "$GPVTG"){
        //cout << "got VTG message\n";
        ExtractVTGData();
        return;
    }else if(MsgType == "$PGRME"){
        //cout << "got RME message\n";
        ExtractRMEData();
        return;
    }else if(MsgType == "$PGRMO"){
        //This message is sent by us to control msg transmission and then echoed by GPS
        //So we can just ignore it
        return;
    }else{
        // if we get here the msg is unknown
        stringstream  ErrMsg; 
        ErrMsg << "Message type unknown " << MsgType <<endl; 
        throw GpsException(ErrMsg.str());
    }

    
}


//**************************************************************************************
// Get the useful bits from a GGA message

void 
GarminGpsDriver::ExtractGGAData(void){

    //Names for the tokens in the GGA message
    enum GGATokens{MsgType=0,UTC,Lat,LatDir,Lon,LonDir,FixType,
                   NSatsUsed,HDOP,Hgt,M1,GeoidHgt,M2,DiffAge,DiffId};

    //cout << nmeaMessage_.sentence()<<endl;

    //position fix type
    switch (nmeaMessage_.getDataToken(FixType)[0])
    {
    case '0': 
        GpsData_.positionType = orca::GpsPositionTypeNotAvailable;
        hasFix_ = false;
        return;
    case '1': 
        GpsData_.positionType = orca::GpsPositionTypeAutonomous;
        break;
    case '2': 
        GpsData_.positionType = orca::GpsPositionTypeDifferential;
        break;
    }
    
    GpsData_.timeStamp = orcaice::toOrcaTime (timeOfRead_);
    
    //UTC time 
    sscanf(nmeaMessage_.getDataToken(UTC).c_str(),"%02d%02d%lf",
           &GpsData_.utcTime.hours, &GpsData_.utcTime.minutes, &GpsData_.utcTime.seconds);
    //position
    int deg;
    double min;
    double dir;
    
    //latitude
    sscanf(nmeaMessage_.getDataToken(Lat).c_str(),"%02d%lf",&deg,&min);
    dir = (*nmeaMessage_.getDataToken(LatDir).c_str()=='N') ? 1.0 : -1.0;
    GpsData_.latitude=dir*(deg+(min/60.0));
    //longitude
    sscanf(nmeaMessage_.getDataToken(Lon).c_str(),"%03d%lf",&deg,&min);
    dir = (*nmeaMessage_.getDataToken(LonDir).c_str()=='E') ? 1.0 : -1.0;
    GpsData_.longitude=dir*(deg+(min/60.0));
    
    //number of satellites in use
    GpsData_.satellites = atoi(nmeaMessage_.getDataToken(NSatsUsed).c_str());
    
    //altitude
    GpsData_.altitude=atof(nmeaMessage_.getDataToken(Hgt).c_str());
    
    //geoidal Separation
    GpsData_.geoidalSeparation=atof(nmeaMessage_.getDataToken(GeoidHgt).c_str());
    

    //cout << "Lat " << GpsData_.latitude << " Long " << GpsData_.longitude ;
    //cout << " Hght "<< GpsData_.altitude << " Geoid "<< GpsData_.geoidalSeparation << endl;

    // Set flag
    newGpsData_=true;
    hasFix_=true;
    
    return;
}


//********************************************************************
// VTG provides velocity and heading information
void 
GarminGpsDriver::ExtractVTGData(void){

    //Names for the VTG message items
    enum VTGTokens{MsgType=0,HeadingTrue,T,HeadingMag,M,SpeedKnots,
                   N,SpeedKPH,K,ModeInd};

    newGpsData_=true;

    //Check for an empty string. Means that we are not moving
    //When the message has empty fields tokeniser skips so we get the next field inline.
    if(nmeaMessage_.getDataToken(HeadingTrue)[0] == 'T' ){
        GpsData_.speed=0.0;
        GpsData_.climbRate=0.0;
        GpsData_.heading=0.0;
        return;
    }

    //heading
    double headingRad = DEG2RAD(atof(nmeaMessage_.getDataToken(HeadingTrue).c_str()));
    NORMALISE_ANGLE( headingRad );
    GpsData_.heading=headingRad;
    //speed - converted to m/s
    GpsData_.speed=atof(nmeaMessage_.getDataToken(SpeedKPH).c_str());
    GpsData_.speed*=(1000/3600.0);
    //set to zero
    GpsData_.climbRate=0.0;
    
    //cout << nmeaMessage_.sentence() << endl;
    // cout << "head "<< RAD2DEG(GpsData_.heading) << " speed " << GpsData_.speed << endl;
    
    return;

}


//*********************************************************************************************
// RME message. This one is garmin specific... Give position error estimates

void 
GarminGpsDriver::ExtractRMEData(void){
    //Names for the RME message items
    enum VTGTokens{MsgType=0,HError,M1,VError,M2,EPE,M3};
    
    newGpsData_=true;

    GpsData_.horizontalPositionError = atof(nmeaMessage_.getDataToken(HError).c_str());
    GpsData_.verticalPositionError = atof(nmeaMessage_.getDataToken(VError).c_str());
    
    //cout << nmeaMessage_.sentence() << endl;
    //cout << "Herr " << GpsData_.horizontalPositionError << " Verr " << GpsData_.verticalPositionError<<endl;

    return;

}
