/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_WIFI_ICE
#define ORCA2_WIFI_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_wifi Wifi
    @brief Wireless interfaces information

    @{
*/
    
//! Interpretation of link quality
module linkqualitytype
{
    //! link quality type is unknown
    const int UNKNOWN   = 0;
    //! link quality is in dBm
    const int DBM       = 1;
    //! link quality is relative
    const int RELATIVE  = 2;
};

//! Operation modes
module opmode
{
    //! unknown operating mode
    const int UNKNOWN        = 0;
    //! driver decides the mode
    const int AUTO           = 1;
    //! ad hoc mode
    const int ADHOC          = 2;
    //! infrastructure mode (multi cell network, roaming)
    const int INFRASTRUCTURE = 3;
    //! access point, master mode
    const int MASTER         = 4;
    //! repeater mode
    const int REPEAT         = 5;
    //! secondary/backup repeater
    const int SECONDREPEATER = 6;
};



/*!
 *  @brief Contains information per wireless interface, e.g. eth1.
 * 
 *  All values are driver specific. This data structure resembles information from /proc/net/wireless
 *  plus some information obtained from the kernel.
 *  Read man-page of iwconfig for detailed documentation of all fields.
 * 
 */
struct WifiInterface
{
    //! Interface name, e.g. eth1
    string interfaceName;
    //! Status (device-dependent)
    int status;
    //! Link quality
    int linkQuality;
    //! Signal level
    int signalLevel;
    //! Noise level
    int noiseLevel;
    //! Rx invalid nwid: number of packets received with a different NWID or ESSID.
    int numInvalidNwid;
    //! Rx invalid crypt: number of packets hardware was unable to decrypt
    int numInvalidCrypt;
    //! Rx invalid frag: number of packets not properly reassembled
    int numInvalidFrag;
    //! Tx excessive retries: number of packets hardware failed to deliver
    int numRetries;
    //! Invalid misc: other lost packets
    int numInvalidMisc;
    //! Missed beacon: number of periodic beacons from cell/access point we have missed
    int numMissedBeacons;
    
    //! Operating mode (see above)
    int mode;
    //! Bitrate
    int bitrate;
    //! Access point MAC address
    string accessPoint;
    
    //! Throughput (TODO: what does it mean? unit?)
    int throughPut;
    //! How to interpret the link quality (see above)
    int linkQualityType;
    //! Maximum link quality
    int maxLinkQuality;
    //! Maximum signal level
    int maxSignalLevel;
    //! Maximum noise level
    int maxNoiseLevel;
    
};

//! A data structure which keeps individual wifi interfaces
sequence<WifiInterface> WifiInterfaces;

/*!
    @brief Wireless signal data
*/
struct WifiData
{
    //! Time when data was measured.
    Time timeStamp;
    //! A sequence of wireless interfaces
    WifiInterfaces interfaces;
};

/*!
    @brief Subscriber to Wifi information.
*/
interface WifiConsumer
{
    //! Set data
    void setData( WifiData obj );
};

//! Interface to Wifi information.
interface Wifi
{
    //! Get current Wifi data. Raises DataNotExistException if data is not available.
    nonmutating WifiData getData()
        throws DataNotExistException;

    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @see unsubscribe
     */
    void subscribe( WifiConsumer* subscriber )
            throws ConfigurationNotExistException;

    // for reference, this is what IceStorm's subscribe function looks like.
    //void subscribe(QoS theQoS, Object* subscriber);

    /*!
     * Unsubscribe the given [subscriber].
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( WifiConsumer* subscriber );
};

/*! @} */
}; // module

#endif
