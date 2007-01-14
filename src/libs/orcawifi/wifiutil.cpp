#include <netinet/in.h>
#include <linux/wireless.h>
#include <sys/ioctl.h>
#include <netinet/if_ether.h>

#include <iostream>

// only for modes/linkqualitytype enums
#include <orca/wifi.h>

#include "wifiutil.h"

using namespace std;
using namespace orca;

namespace wifiutil {

void getInterfaceNames( vector<string> &interfaceNames )
{
    FILE *wifiInfoFile;
    char buf[128];
    
    int numTries=0;
    while(1)
    {
        // get the wireless device from /proc/net/wireless
        if ((wifiInfoFile = fopen("/proc/net/wireless", "r")) == NULL) 
        {
            numTries++;
            cout << "Warning(getInterfaceNames): Couldn't open for reading. Tried " << numTries << " times." << endl;
            if (numTries==5) throw Exception( "Error:\nCouldn't open /proc/net/wireless for reading.\n" );
        } else {
            break;
        }
    }
        
    // lets skip the header
    for (int i = 0; i < 3; i++) 
    {    
        if (fgets(buf, sizeof(buf),wifiInfoFile) == NULL) {
            fclose(wifiInfoFile);
            throw NoWirelessInterfaceException( "No wireless interface available\n" );
        }
    }
    
    int numEth;
    
    while (true)
    {
        sscanf(buf, "  eth%d:", &numEth);
        stringstream ss; 
        ss << "eth" << numEth;
        interfaceNames.push_back( ss.str() );
        // read the next line
        if (fgets(buf, sizeof(buf),wifiInfoFile) == NULL) break;
    }
    
    fclose( wifiInfoFile );
}


void readFromProc( vector<ProcData> &wifiData )
{  
    FILE *wifiInfoFile;
    ProcData data;
    char buf[128];
    
    int numTries=0;
    
    while(1)
    {
        // get the wireless device from /proc/net/wireless
        if ((wifiInfoFile = fopen("/proc/net/wireless", "r")) == NULL) 
        {
            numTries++;
            cout << "Warning(readFromProc): Couldn't open for reading. Tried " << numTries << " times." << endl;
            if (numTries==5) throw Exception( "Error:\nCouldn't open /proc/net/wireless for reading.\n" );
        } else {
            break;
        }
    }  
        
    // lets skip the header
    for (int i = 0; i < 3; i++) 
    {    
        if (fgets(buf, sizeof(buf),wifiInfoFile) == NULL) {
            fclose(wifiInfoFile);
            throw NoWirelessInterfaceException( "No wireless interface available\n");
        }
    }
    
    while (true)
    {
        char *bp;
        int numEth;
        bp=buf;
        while(*bp&&isspace(*bp)) bp++;
        
        /* Get interface names */
        sscanf(buf, "  eth%d:", &numEth);
        stringstream ss; 
        ss << "eth" << numEth;
        data.interfaceName = ss.str();
    
        /* Skip ethX: */
        bp=strchr(bp,':');
        bp++;
        
        /* -- status -- */
        bp = strtok(bp, " ");
        sscanf(bp, "%X", &data.status);
        
        /* -- link quality -- */
        bp = strtok(NULL, " ");
        if(strchr(bp,'.') != NULL)
        sscanf(bp, "%d", &data.linkQuality);
        
        /* -- signal level -- */
        bp = strtok(NULL, " ");
        if(strchr(bp,'.') != NULL)
        sscanf(bp, "%d", &data.signalLevel);
          
        /* -- noise level -- */
        bp = strtok(NULL, " ");
        if(strchr(bp,'.') != NULL)
        sscanf(bp, "%d", &data.noiseLevel);
        
        /* -- discarded packets -- */
        bp = strtok(NULL, " ");
        sscanf(bp, "%d", &data.numInvalidNwid);
        bp = strtok(NULL, " ");
        sscanf(bp, "%d", &data.numInvalidCrypt);
        bp = strtok(NULL, " ");
        sscanf(bp, "%d", &data.numInvalidFrag);
        bp = strtok(NULL, " ");
        sscanf(bp, "%d", &data.numRetries);
        bp = strtok(NULL, " ");
        sscanf(bp, "%d", &data.numInvalidMisc);
        bp = strtok(NULL, " ");
        sscanf(bp, "%d", &data.numMissedBeacons);  
        
        wifiData.push_back( data );
        
        // read the next line
        if (fgets(buf, sizeof(buf),wifiInfoFile) == NULL) break;
    }
    
    fclose(wifiInfoFile);
}


void printData( vector<ProcData> &wifiData )
{    
    for ( uint i=0; i < wifiData.size(); i++ )
    {
        const ProcData &p = wifiData[i]; 
        cout << "Interface name:       " << p.interfaceName << endl;
        cout << "Status:               " << p.status << endl;
        cout << "Link quality:         " << p.linkQuality << endl;
        cout << "Signal level:         " << p.signalLevel << endl;
        cout << "Noise level:          " << p.noiseLevel << endl;
        cout << "Rx invalid nwid:      " << p.numInvalidNwid << endl;
        cout << "Rx invalid crypt:     " << p.numInvalidCrypt << endl;
        cout << "Rx invalid frag:      " << p.numInvalidFrag << endl;
        cout << "Tx excessive retries: " << p.numRetries << endl;   
        cout << "Invalid misc:         " << p.numInvalidMisc << endl;   
        cout << "Missed beacon:        " << p.numMissedBeacons << endl << endl;        
    }
}

void printConfig( vector<WirelessConfig> &wifiConfigs )
{    
    for ( uint i=0; i < wifiConfigs.size(); i++ )
    {
        const WirelessConfig &w = wifiConfigs[i]; 
        cout << "Interface name:     " << w.interfaceName << endl;
        cout << "Mode:               " << w.mode << endl;
        cout << "Bitrate:            " << w.bitrate << endl;
        cout << "Access point:       " << w.accessPoint << endl << endl;        
    }
}

void printDataIoctl( vector<IoctlData> &wifiDataIoctl )
{    
    for ( uint i=0; i < wifiDataIoctl.size(); i++ )
    {
        const IoctlData &d = wifiDataIoctl[i]; 
        cout << "Interface name:     " << d.interfaceName << endl;
        cout << "Throughput:         " << d.throughPut << endl;
        cout << "Link quality type:  " << d.linkQualityType << endl;
        cout << "Link quality:       " << d.linkQuality << endl;
        cout << "Max link quality:   " << d.maxLinkQuality << endl;
        cout << "Signal level:       " << d.signalLevel << endl;
        cout << "Max signal level:   " << d.maxSignalLevel << endl;
        cout << "Noise level:        " << d.noiseLevel << endl;
        cout << "Max noise level:    " << d.maxNoiseLevel << endl << endl;    
    }
        
}

char* convertMacHex(char *buf, unsigned char *data)
{
  struct ether_addr * p = (struct ether_addr *)data;
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
      p->ether_addr_octet[0], p->ether_addr_octet[1],
      p->ether_addr_octet[2], p->ether_addr_octet[3],
      p->ether_addr_octet[4], p->ether_addr_octet[5]);

  return buf;
}


int getMode( int socketFd, iwreq* iwRequest )
{
    int mode;
    
    if (ioctl(socketFd, SIOCGIWMODE, iwRequest) < 0)
        throw Exception( "Error:\nIoctl for mode failed.\n" );
        
    switch(iwRequest->u.mode) {
        case IW_MODE_AUTO:
            mode = opmode::AUTO;
            break;
        case IW_MODE_ADHOC:
            mode = opmode::ADHOC;
            break;
        case IW_MODE_INFRA:
            mode = opmode::INFRASTRUCTURE;
            break;
        case IW_MODE_MASTER:
            mode = opmode::MASTER;
            break;
        case IW_MODE_REPEAT:
            mode = opmode::REPEAT;
            break;
        case IW_MODE_SECOND:
            mode = opmode::SECONDREPEATER;
            break;
        default:
            mode = opmode::UNKNOWN;
    }    
    return mode;
}

string getAccessPointMac( int socketFd, iwreq* iwRequest )
{
    char accessPoint[32];
    
    if (ioctl(socketFd, SIOCGIWAP, iwRequest) < 0)
        throw Exception( "Error:\nIoctl for access point failed.\n" );
    
    struct sockaddr sa;
    memcpy(&sa, &(iwRequest->u.ap_addr), sizeof(sa));
    convertMacHex(accessPoint, (unsigned char *)sa.sa_data);
    
    return string( accessPoint );
}

int getBitrate( int socketFd, iwreq* iwRequest )
{
    if (ioctl(socketFd, SIOCGIWRATE, iwRequest) < 0) 
        throw Exception( "Error:\nIoctl for bitrate failed.\n" );
    
    int bitrate = iwRequest->u.bitrate.value;
    
    return bitrate;
}

void readConfig( vector<WirelessConfig> &wifiConfigs )
{
    // get a list of all current wireless interfaces
    vector<string> interfaceNames;
    getInterfaceNames(interfaceNames);  
    
    struct iwreq* iwRequest = new struct iwreq;
    
    // this is the socket to use for wireless info
    int socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd < 0)
    {
        cout << "Didn't get a socket!" << endl;
        throw Exception( "Error:\nUnable to get a socket.\n" );
    }
    
    WirelessConfig cfg;
    
    for (uint i=0; i<interfaceNames.size(); i++)
    {
        cfg.interfaceName = interfaceNames[i];
        
        char interface_name[32]; // name of wireless device
        strncpy(interface_name,interfaceNames[i].c_str(),sizeof(interface_name));
        
        // IMPORTANT: copy this name into the iwreq struct for use in ioctls
        strncpy(iwRequest->ifr_name, interface_name, IFNAMSIZ);
        
        // Request for mode 
        cfg.mode = getMode( socketFd, iwRequest );
        
        // Request for access point address
        cfg.accessPoint = getAccessPointMac( socketFd, iwRequest );
         
        // Request for bitrate
        cfg.bitrate = getBitrate( socketFd, iwRequest );
        
        wifiConfigs.push_back( cfg );
    }
    
    close(socketFd);
}

void readUsingIoctl( vector<IoctlData> &wifiDataIoctl )
{
    struct iwreq* iwRequest = new struct iwreq;
    struct iw_range* iwRange = new struct iw_range;
    struct iw_statistics* iwStats = new struct iw_statistics;  
    
    // this is the socket to use for wireless info
    int socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd < 0)
    {
        cout << "Didn't get a socket!" << endl;
        throw Exception( "Error:\nUnable to get a socket.\n" );
    }
    
    // get a list of all current wireless interfaces
    vector<string> interfaceNames;
    getInterfaceNames(interfaceNames);
    
    IoctlData data;
    
    for (uint i=0; i<interfaceNames.size(); i++)
    {
        data.interfaceName = interfaceNames[i];
        
        char interface_name[32]; // name of wireless device
        strncpy(interface_name,interfaceNames[i].c_str(),sizeof(interface_name));
        
        // copy this name into the iwreq struct for use in ioctls
        strncpy(iwRequest->ifr_name, interface_name, IFNAMSIZ);
        
        // ========== Request for range ===============
        struct iw_range requestBuffer;
        iwRequest->u.data.pointer = (caddr_t)&requestBuffer;
        iwRequest->u.data.length = sizeof(struct iw_range);
        iwRequest->u.data.flags = 0;
        
        if(ioctl(socketFd, SIOCGIWRANGE, iwRequest) >= 0) 
        {
            memcpy((char *) iwRange, &requestBuffer, sizeof(struct iw_range));
        }
        else
        {
            throw Exception( "Error:\nIoctl for range failed.\n" );    
        }
        
        // ============= Request for stats ================
        struct iw_quality *qual;
        iwRequest->u.data.pointer = (caddr_t) iwStats;
        iwRequest->u.data.length = sizeof(struct iw_statistics);
        iwRequest->u.data.flags = 1;  // clear updated flag
        if(ioctl(socketFd, SIOCGIWSTATS, iwRequest) >= 0) 
        {
            qual = &iwStats->qual;
            data.throughPut = iwRange->throughput;
            if (qual->level != 0) {
                if (qual->level > iwRange->max_qual.level) {
                    data.linkQualityType = linkqualitytype::DBM;
                } else {
                    data.linkQualityType = linkqualitytype::RELATIVE;
                }
            }
            else
            {
                data.linkQualityType = linkqualitytype::UNKNOWN;
            }
        
            data.linkQuality = qual->qual;
            data.maxLinkQuality = iwRange->max_qual.qual;
            data.signalLevel = qual->level;
            data.maxSignalLevel = iwRange->max_qual.level;
            data.noiseLevel = qual->noise;
            data.maxNoiseLevel = iwRange->max_qual.noise;
        }
        else
        {
            throw Exception( "Error:\nIoctl for stats failed.\n" );    
        }
    
        wifiDataIoctl.push_back( data );
    }
    
    close(socketFd);
}

}

