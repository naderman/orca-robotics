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
#include <orcaobj/initutils.h>
#include "hardwaredriver.h"

using namespace std;

namespace wifi
{
        
// Fills the config entries into the data
// Extends the data.interfaces vector if it's smaller than the config vector
void fillIn( const vector<wifiutil::WirelessConfig> &config,
             orca::WifiData &data )
{
    for (unsigned int i=0; i<config.size(); i++)
    {
        orca::WifiInterface w;
        
        if ( i<data.interfaces.size() ) {
            w = data.interfaces[i];
        } else {
            orcaice::setInit(w);
            data.interfaces.push_back(w);
        }
        
        w.mode = config[i].mode;
        w.bitrate = config[i].bitrate;
        w.accessPoint = config[i].accessPoint;
          
        data.interfaces[i] = w;
    }
}

// Fills the procData entries into the data
// Extends the data.interfaces vector if it's smaller than the procData vector
void fillIn( const vector<wifiutil::ProcData> &procData,
             orca::WifiData &data )
{
    for (unsigned int i=0; i<procData.size(); i++)
    {
        orca::WifiInterface w;
        
        if ( i<data.interfaces.size() ) {
            w = data.interfaces[i];
        } else {
            orcaice::setInit(w);
            data.interfaces.push_back(w);
        }
        
        w.interfaceName = procData[i].interfaceName;
        w.status = procData[i].status;
        w.linkQuality = procData[i].linkQuality;
        w.signalLevel = procData[i].signalLevel;
        w.noiseLevel = procData[i].noiseLevel;
        w.numInvalidNwid = procData[i].numInvalidNwid;
        w.numInvalidCrypt = procData[i].numInvalidCrypt;
        w.numInvalidFrag = procData[i].numInvalidFrag;
        w.numRetries = procData[i].numRetries;
        w.numInvalidMisc = procData[i].numInvalidMisc;
        w.numMissedBeacons = procData[i].numMissedBeacons;
          
        data.interfaces[i] = w;
    }
        
}

// Fills the ioctl data entries into the data
// Extends the data.interfaces vector if it's smaller than the ioctlData vector
void fillIn( const vector<wifiutil::IoctlData> &ioctlData,
             orca::WifiData &data )
{    
    for (unsigned int i=0; i<ioctlData.size(); i++)
    {
        orca::WifiInterface w;
        
        if ( i<data.interfaces.size() ) {
            w = data.interfaces[i];
        } else {
            orcaice::setInit(w);
            data.interfaces.push_back(w);
        }
        
        w.throughPut = ioctlData[i].throughPut;
        w.linkType = ioctlData[i].linkType;
        w.linkQuality = ioctlData[i].linkQuality;
        w.signalLevel = ioctlData[i].signalLevel;
        w.noiseLevel = ioctlData[i].noiseLevel;
        w.maxLinkQuality = ioctlData[i].maxLinkQuality;
        w.maxSignalLevel = ioctlData[i].maxSignalLevel;
        w.maxNoiseLevel = ioctlData[i].maxNoiseLevel;
          
        data.interfaces[i] = w;
    }
}

HardwareDriver::HardwareDriver(orcaice::Context &context)
  : context_(context),
    haveConfig_(false)
{
}

void
HardwareDriver::init()
{
    try
    {
        wifiutil::readConfig( config_ );
        haveConfig_ = true;
    }
    catch (wifiutil::Exception &e) 
    {
        stringstream ss;
        ss << "HardwareDriver: Caught exception: " << e.what();
        ss << "We can't read using ioctl. Will just read from proc in the future"; 
        context_.tracer()->debug( ss.str(), 1 );
    }
  
}
  
void 
HardwareDriver::read( orca::WifiData &data )
{
    
    // set the timestamp
    data.timeStamp = orcaice::getNow();
    
    // ======= proc data ============
    vector<wifiutil::ProcData> procData;
    wifiutil::readFromProc( procData );
    fillIn( procData, data );
    
    // if we didn't manage to get the config, reading from ioctl doesn't work and we just return 
    if (!haveConfig_) return;
    
    // assumption: config does not change
    fillIn( config_, data );
    
    // ======== ioctl data =============
    bool haveIoctl = true;
    vector<wifiutil::IoctlData> ioctlData;
    try 
    {
        wifiutil::readUsingIoctl( ioctlData );
    } 
    catch (wifiutil::Exception &e) 
    {
        stringstream ss;
        ss << "HardwareDriver: Caught exception: " << e.what();
        ss << "We'll continue because this operation is not critical" << endl;      
        context_.tracer()->info( ss.str() );
        haveIoctl = false;
    }
    // if we managed to get information from ioctl, we'll add and refine the data
    if (haveIoctl) fillIn( ioctlData, data );
    
}
            
}           
