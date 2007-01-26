/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GPS_INTERFACE_ICE
#define ORCA2_GPS_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_gps Gps
    @brief Access to GPS devices.
    @{
*/

// define messages first

//! Gps config structure
struct GpsDescription
{
    //! Time (according to the computer clock) when data was measured.
    Time timeStamp;

    //! Offset of the sensor w.r.t to the global coordinate system.
    Frame3d offset; 

    //! Dimensions of the sensor
    Size3d  size;
};

//! Gps time structure - for time sync
struct GpsTimeData
{
    //! Time (according to the computer clock) when data was measured.
    Time timeStamp;
    //! UTC time
    TimeOfDay utcTime;
    //! UTC date
    Date utcDate;
};

//! Gps data structure
struct GpsMapGridData
{
    //! Time (according to the computer clock) when data was measured.
    Time timeStamp;
    //! UTC time
    TimeOfDay utcTime;

    // GPS position
    //! Our current zone
    int zone;
    //! Northing (Metres)
    double northing;
    //! Easting (Metres)
    double easting;
    //! Altitude (Metres above Ellipsoid)
    double altitude;
    
    // Velocities
    //! heading/track/course (Degrees)
    double heading; 
    //! horizontal speed (Metres/second)
    double speed;
    //! vertical velocity (Metres/second)
    double climbRate;
    
    //! Position Type (Bad (0), Ugly (1), Good (2))
    int positionType;
};


//! Gps data structure
struct GpsData
{
    //! Time (according to the computer clock) when data was measured.
    Time timeStamp;
    //! UTC time
    TimeOfDay utcTime;
 
    // GPS position
    //! Latitude (Degrees)
    double latitude;
    //! Longitude (Degrees)
    double longitude;
    //! Altitude (Metres above Ellipsoid)
    double altitude;
    
    // Velocities
    //! heading/track/course (Degrees)
    double heading; 
    //! horizontal speed (Metres/second)
    double speed;
    //! vertical velocity (Metres/second)
    double climbRate;
    
    //! Number of satellites
    int satellites;
    //! Position Type (Bad (0), Ugly (1), Good (2))
    int positionType;
    //! Geoidal Separation (Metres)
    double geoidalSeparation;    
};


/*!
 *
 * Data consumer interface (needed only for the push pattern).
 *
 * In Orca-1 terms, this the Consumer side of the ClientPush interface.
 *
 */
interface GpsConsumer
{
    void setData( GpsData obj );
};

/*!
 * Gps Map information consumer interface
 */
interface GpsMapGridConsumer
{
    void setData( GpsMapGridData obj );
};

/*!
 * Gps Time information consumer interface
 */
interface GpsTimeConsumer
{
    void setData( GpsTimeData obj );
};

/*!
    @brief Access to GPS data.
*/
interface Gps
{
    //! Returns the latest data.
    //! @note In Orca1 this would be called ClientPull_Supplier interface.
    nonmutating GpsData getData()
            throws HardwareFailedException;

    //! Return the latest timestamp information
    nonmutating GpsTimeData getTimeData()
            throws HardwareFailedException;

    //! Return the latest map information
    nonmutating GpsMapGridData getMapGridData()
            throws HardwareFailedException;

    nonmutating GpsDescription getDescription();

    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @see unsubscribe
     */
    void subscribe( GpsConsumer* subscriber )
            throws SubscriptionFailedException;

    void subscribeForMapGrid( GpsMapGridConsumer* subscriber )
            throws SubscriptionFailedException;

    void subscribeForTime( GpsTimeConsumer* subscriber )
            throws SubscriptionFailedException;

    // for reference, this is what IceStorm's subscribe function looks like.
    //void subscribe(QoS theQoS, Object* subscriber);

    /*!
     * Unsubscribe the given [subscriber].
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( GpsConsumer* subscriber );
    
    idempotent void unsubscribeForMapGrid( GpsMapGridConsumer* subscriber );
    
    idempotent void unsubscribeForTime( GpsTimeConsumer* subscriber );
};


//!  //@}
}; // module

#endif
