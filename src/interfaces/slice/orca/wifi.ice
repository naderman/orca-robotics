/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
    @brief Wireless signal data

    @{
*/

/*!
    @brief Wireless signal information
*/
// struct WifiInfo
// {
//     //! Time when info was compiled.
//     Time timeStamp;
//     //! Unparsed string with some wifi information, e.g. output of uname in Linux.
//     string info;
// };

/*!
 *  @brief Describes the data per wireless link.
 * 
 */
struct WifiLink
{
  /** MAC address. */
  int macCount;
  string macAddress;
  /** IP address. */
  int ipCount;
  string ipAddress;
  /** ESSID. */
  int essidCount;
  string essid;
  /** Mode (master, adhoc, etc). */
  int mode;
  /** Frequency [MHz]. */
  int frequency;
  /** Encryted?. */
  int encrypt;
  /** Link quality */
  int linkQuality;
  /** Link level */
  int linkLevel;
  /** Link noise */
  int linkNoise;
};

//! A data structure which keeps individual wifi links
sequence<WifiLink> WifiLinks;

/*!
    @brief Wireless signal data
*/
struct WifiData
{
    //! Time when data was measured.
    Time timeStamp;
    /** length of said list */
    int numLinks;
    /** A list of links */
    WifiLinks wirelessLinks;
    /** mysterious throughput calculated by driver */
    int throughput;
    /** current bitrate of device */
    int bitrate;
    /** operating mode of device */
    int mode;
    /** Indicates type of link quality info we have */
    int linkQualityType;
    /** Maximum value for quality */
    int maxLinkQuality;
    /** Maximum value for level */
    int maxLinkLevel;
    /** Maximum value for noise. */
    int maxNoiseLevel;
    /** MAC address of current access point/cell */
    string accessPointMacAddress;
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
    //! Get general Wifi information.
//     nonmutating CpuInfo getInfo();

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
