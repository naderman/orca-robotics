/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include "wifi.h"
#include "datetime.h"

#include <orcaifaceutil/wifi.h>

using namespace std;

namespace orcaobj
{

// alexm: sparing it for now because it has a non-zero entry.
void setInit( orca::WifiInterface &w )
{
    w.mode = orca::OperationModeUnknown;  
    w.bitrate = 0;  
    w.accessPoint = "00:00:00:00:00:00";  
    w.interfaceName = "";  
    w.status = 0;  
    w.linkQuality = 0;  
    w.signalLevel = 0;  
    w.noiseLevel = 0;  
    w.numInvalidNwid = 0;  
    w.numInvalidCrypt = 0;  
    w.numInvalidFrag = 0;  
    w.numRetries = 0;  
    w.numInvalidMisc = 0;  
    w.numMissedBeacons = 0;  
    w.throughPut = 0;
    w.linkType = orca::LinkQualityTypeUnknown;
    w.maxLinkQuality = 0;
    w.maxSignalLevel = 0;
    w.maxNoiseLevel = 0;
}

std::string 
toString( const orca::WifiData &obj )
{
    std::ostringstream s;
    // first line: timestamp
    s << toString(obj.timeStamp) << " WifiData [" << obj.interfaces.size() << " interfaces]: " << endl;
    
    for ( unsigned int i=0; i<obj.interfaces.size(); ++i )
    {
        const orca::WifiInterface &iface = obj.interfaces[i];
        
        s << "name: " << iface.interfaceName 
          << ", status: " << iface.status 
          << ", link quality: " <<  iface.linkQuality 
          << ", signal level: " << iface.signalLevel 
          << ", noise level: " << iface.noiseLevel << "\n"; 
        
        s << "numInvalidNwid: " << iface.numInvalidNwid
          << ", numInvalidCrypt: " << iface.numInvalidCrypt
          << ", numInvalidFrag: " << iface.numInvalidFrag
          << ", numRetries: " << iface.numRetries
          << ", numInvalidMisc: " << iface.numInvalidMisc
          << ", numMissedBeacons: " << iface.numMissedBeacons << "\n";
          
        s << "mode: " << ifaceutil::toString(iface.mode) 
          << ", bitrate: " << iface.bitrate 
          << ", accessPoint: " << iface.accessPoint 
          << ", throughPut: " << iface.throughPut 
          << ", linkQualityType: " << ifaceutil::toString(iface.linkType) << "\n";
        
        s << "maxLinkQuality: " << iface.maxLinkQuality 
          << ", maxSignalLevel: " << iface.maxSignalLevel 
          << ", maxNoiseLevel: " << iface.maxNoiseLevel << "\n\n";
    }
            
    return s.str();
}

} // namespace
