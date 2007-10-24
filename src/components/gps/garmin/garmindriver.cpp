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
using namespace hydroserial;
using namespace orcagpsutil;
using namespace gps;


//TODO. Duncan needs to re-add the date functionality

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
    //Make sure that we clear our internal data structures
    memset((void*)(&nmeaMessage_) , 0 , sizeof(nmeaMessage_));
    memset((void*)(&gpsData_) , 0 , sizeof(gpsData_));

    try {
        enableDevice();
        //TODO Need to check here that we have been successful.
        clearFrame();
    }
    catch ( const hydroserial::SerialException &e )
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




//****************************************************************************
// Read one complete frame of data. IE all the messages that we need before returning the data.

void
GarminGpsDriver::readFrame(orca::GpsData& GpsData)
{
    
    char serial_data[1024];
    int gpsMsgNotYetGotFrameCount = 0;

    //How many messages are we looking for to make our frame
    const int N_MSGS_IN_FRAME = 3;
    
    //Clear our data before we start trying to assemble the frame
    clearFrame();

    
    while(! haveCompleteFrame() ){
        
        // This will block up to the timeout
        context_.tracer()->debug( "GarminGpsDriver::read(): calling serial_.readLine()", 10 );
        int ret = serial_.readLine(serial_data,1024,'\n');
        context_.tracer()->debug( serial_data, 10 );

        timeOfRead_ = IceUtil::Time::now();
        
        if ( ret<0 ) {
            throw GpsException( "GarminGpsDriver: Timeout reading from serial port" );
        }
    
        if(ret==0) {
            throw GpsException("GarminGpsDriver: Read 0 bytes from serial port");
        }
    
        // 
        // We successfully read something from the serial port
        //
    

        //Put it into the message object and checksum the data
        static int nmeaExceptionCount =0;
        try{
            //This throws if it cannot find the * to deliminate the checksum field
            nmeaMessage_.setSentence(serial_data,TestChecksum);
        }
        catch (NmeaException &e){
        //Don't throw if only occasional messages are missing the checksums
            if(nmeaExceptionCount++ < 3) {return;}
            stringstream ss;
            ss << "MainLoop: Problem reading from GPS: " << e.what();
            context_.tracer()->error( ss.str() );
            throw GpsException(ss.str());
        }
        nmeaExceptionCount = 0;
    
        //Only populate the data structures if our message passes the checksum!
        static int nmeaFailChecksumCount =0;
        if(nmeaMessage_.haveValidChecksum()){        
            nmeaFailChecksumCount = 0;
            addDataToFrame();
        }else{      
            if(nmeaFailChecksumCount++ >= 3){ //Dont throw an exception on the first failed checksum.
                throw GpsException("GarminGpsDriver: more than 3 sequential messages failed the checksum\n");
            }else{
                context_.tracer()->error("GarminGpsDriver: Single message failed checksum. Not throwing an exception yet!\n" );
            }
        }

        //Make sure that we do not wait for ever trying to get a frame of data
        //Note that we might need to skip the N * $PGRMO messages echoed back from receiver when starting
        //As well as the N * messages that we are looking for
        if(gpsMsgNotYetGotFrameCount++ >= (N_MSGS_IN_FRAME * 3)){
            throw GpsException("GarminGpsDriver: Not able to assemble a complete data frame\n");
        }

    }

    context_.tracer()->debug("GPS got a complete frame\n", 10 );

    // Hand the data back to the outside world
    GpsData=gpsData_;

}




//**********************************************************************************

void
GarminGpsDriver::addDataToFrame()
{
    //First split up the data fields in the string we have read.
    nmeaMessage_.parseTokens();
    
    //We should not be being passed any messages with failed checksums, but just in case
    if(nmeaMessage_.haveTestedChecksum() && (!nmeaMessage_.haveValidChecksum())){
        throw GpsException("GarminGpsDriver: Message fails checksum");
    }

    //And then find out which type of messge we have recieved...
    string MsgType = nmeaMessage_.getDataToken(0);
    
    if(MsgType == "$GPGGA"){
        context_.tracer()->debug("got GGA message\n",4);
        extractGGAData();
        haveGGA_ = true;
        return;
    }else if(MsgType == "$GPVTG"){
        context_.tracer()->debug("got VTG message\n",4);
        extractVTGData();
        haveVTG_ = true;
        return;
    }else if(MsgType == "$PGRME"){
        context_.tracer()->debug("got RME message\n",4);
        extractRMEData();
        haveRME_ = true;
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
GarminGpsDriver::extractGGAData(void){

    //Names for the tokens in the GGA message
    enum GGATokens{MsgType=0,UTC,Lat,LatDir,Lon,LonDir,FixType,
                   NSatsUsed,HDOP,Hgt,M1,GeoidHgt,M2,DiffAge,DiffId};

    //cout << nmeaMessage_.sentence()<<endl;

    //position fix type
    switch (nmeaMessage_.getDataToken(FixType)[0])
    {
    case '0': 
        gpsData_.positionType = orca::GpsPositionTypeNotAvailable;
        return;
    case '1': 
        gpsData_.positionType = orca::GpsPositionTypeAutonomous;
        break;
    case '2': 
        gpsData_.positionType = orca::GpsPositionTypeDifferential;
        break;
    }
    
    gpsData_.timeStamp = orcaice::toOrcaTime (timeOfRead_);
    
    //UTC time 
    sscanf(nmeaMessage_.getDataToken(UTC).c_str(),"%02d%02d%lf",
           &gpsData_.utcTime.hours, &gpsData_.utcTime.minutes, &gpsData_.utcTime.seconds);
    //position
    int deg;
    double min;
    double dir;
    
    //latitude
    sscanf(nmeaMessage_.getDataToken(Lat).c_str(),"%02d%lf",&deg,&min);
    dir = (*nmeaMessage_.getDataToken(LatDir).c_str()=='N') ? 1.0 : -1.0;
    gpsData_.latitude=dir*(deg+(min/60.0));
    //longitude
    sscanf(nmeaMessage_.getDataToken(Lon).c_str(),"%03d%lf",&deg,&min);
    dir = (*nmeaMessage_.getDataToken(LonDir).c_str()=='E') ? 1.0 : -1.0;
    gpsData_.longitude=dir*(deg+(min/60.0));
    
    //number of satellites in use
    gpsData_.satellites = atoi(nmeaMessage_.getDataToken(NSatsUsed).c_str());
    
    //altitude
    gpsData_.altitude=atof(nmeaMessage_.getDataToken(Hgt).c_str());
    
    //geoidal Separation
    gpsData_.geoidalSeparation=atof(nmeaMessage_.getDataToken(GeoidHgt).c_str());
    

    //cout << "Lat " << GpsData_.latitude << " Long " << GpsData_.longitude ;
    //cout << " Hght "<< GpsData_.altitude << " Geoid "<< GpsData_.geoidalSeparation << endl;

    // Set flag
    
    return;
}


//********************************************************************
// VTG provides velocity and heading information
void 
GarminGpsDriver::extractVTGData(void){

    //Names for the VTG message items
    enum VTGTokens{MsgType=0,HeadingTrue,T,HeadingMag,M,SpeedKnots,
                   N,SpeedKPH,K,ModeInd};

    //Check for an empty string. Means that we are not moving
    //When the message has empty fields tokeniser skips so we get the next field inline.
    if(nmeaMessage_.getDataToken(HeadingTrue)[0] == 'T' ){
        gpsData_.speed=0.0;
        gpsData_.climbRate=0.0;
        gpsData_.heading=0.0;
        return;
    }

    //heading
    double headingRad = DEG2RAD(atof(nmeaMessage_.getDataToken(HeadingTrue).c_str()));
    NORMALISE_ANGLE( headingRad );
    gpsData_.heading=headingRad;
    //speed - converted to m/s
    gpsData_.speed=atof(nmeaMessage_.getDataToken(SpeedKPH).c_str());
    gpsData_.speed*=(1000/3600.0);
    //set to zero
    gpsData_.climbRate=0.0;
    
    //cout << nmeaMessage_.sentence() << endl;
    // cout << "head "<< RAD2DEG(GpsData_.heading) << " speed " << GpsData_.speed << endl;
    
    return;

}


//*********************************************************************************************
// RME message. This one is garmin specific... Give position error estimates
// See the file garminErrorPositionEstimate.txt for a discussion of the position errors as
// reported here. Essentially the EPE reported by the garmin is a 1 sigma error (RMS) or a
// 68% confidence bounds.

void 
GarminGpsDriver::extractRMEData(void){
    //Names for the RME message items
    enum VTGTokens{MsgType=0,HError,M1,VError,M2,EPE,M3};
    
    gpsData_.horizontalPositionError = atof(nmeaMessage_.getDataToken(HError).c_str());
    gpsData_.verticalPositionError = atof(nmeaMessage_.getDataToken(VError).c_str());
    
    //cout << nmeaMessage_.sentence() << endl;
    //cout << "Herr " << GpsData_.horizontalPositionError << " Verr " << GpsData_.verticalPositionError<<endl;

    return;

}
