/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef WIFI_UTIL_H
#define WIFI_UTIL_H

#include <string>
#include <vector>

namespace wifiutil {

    enum Result
    {
        NoWirelessInterface,
        IoctlProblem,
        OtherError
    };
    
    //! Exceptions for wifi access 
    class Exception : public std::exception
    {
    public:

        Exception(const char *message)
            : message_(message) {}
        Exception(const std::string &message)
            : message_(message) {}

        virtual ~Exception() throw() {}

        virtual const char* what() const throw() { return message_.c_str(); }
        
        virtual Result type() const { return OtherError; }

    protected:

        std::string  message_;
    };
    
    class NoWirelessInterfaceException : public Exception
    {  
        public:
            NoWirelessInterfaceException( const char * message )
                : Exception( message ) {}
            virtual Result type() const { return NoWirelessInterface; }        
    };
    
    class IoctlException : public Exception
    {  
        public:
            IoctlException( const char * message )
                : Exception( message ) {}
            virtual Result type() const { return IoctlProblem; }        
    };
    
    //! Wireless data structure for data obtainable from /etc/proc
    struct ProcData
    {
        std::string interfaceName;
        int status;
        int linkQuality;
        int signalLevel;
        int noiseLevel;
        int numInvalidNwid;
        int numInvalidCrypt;
        int numInvalidFrag;
        int numRetries;
        int numInvalidMisc;
        int numMissedBeacons;   
    };
    
    //! Operation modes (equivalent to Slice definitions)
    enum OperationMode
    {
        // unknown operating mode
        OperationModeUnknown,
        // driver decides the mode
        OperationModeAuto,
        // ad hoc mode
        OperationModeAdhoc,
        // infrastructure mode (multi cell network, roaming)
        OperationModeInfrastructure,
        // access point, master mode
        OperationModeMaster,
        // repeater mode
        OperationModeRepeat,
        // secondary/backup repeater
        OperationModeSecondRepeater
    };
    
    //! Wireless config data structure
    struct WirelessConfig
    {
        std::string interfaceName;
        OperationMode mode;
        int bitrate;
        std::string accessPoint;
    };
    
    //! Interpretation of link quality (equivalent to Slice definitions)
    enum LinkQualityType 
    {
        // link quality type is unknown
        LinkQualityTypeUnknown,
        // link quality is in dBm
        LinkQualityTypeDbm,
        // link quality is relative
        LinkQualityTypeRelative
    };
    
    //! Wireless data structure for ioctl-obtainable data
    struct IoctlData
    {
        std::string interfaceName;
        int throughPut;
        LinkQualityType linkType;
        int linkQuality;
        int signalLevel;
        int noiseLevel;
        int maxLinkQuality;
        int maxSignalLevel;
        int maxNoiseLevel;
    };
    
    //! Reads wireless information from /proc/net/wireless
    //! One vector entry per interface (e.g. eth1, eth2)
    void readFromProc( std::vector<ProcData> &wifiData );
    
    //! Prints wireless proc data to the screen
    void printData( std::vector<ProcData> &wifiData );
    
    //! Reads wireless configuration information
    //! One vector entry per interface (e.g. eth1, eth2)
    void readConfig( std::vector<WirelessConfig> &wifiConfigs );
    
    //! Prints wirelss configurations to the screen
    void printConfig( std::vector<WirelessConfig> &wifiConfigs );
    
    //! Reads wireless data using a kernel socket (ioctl)
    //! One vector entry per interface (e.g. eth1, eth2)
    void readUsingIoctl( std::vector<IoctlData> &wifiDataIoctl );
    
    //! Prints ioctl data to the screen
    void printDataIoctl( std::vector<IoctlData> &wifiDataIoctl );
    
    //! Gets the names of the current wireless interfaces, e.g. eth1, eth2
    void getInterfaceNames( std::vector<std::string> &interfaceNames );

}
 
#endif
 
