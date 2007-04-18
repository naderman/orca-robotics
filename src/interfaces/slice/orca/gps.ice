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


//! Gps config structure
struct GpsDescription
{
    //! Time (according to the computer clock) when data was measured.
    Time timeStamp;
    
    //! Transformation from global (arbitrarily defined) coordinate system (CS) to
    //! the GPS MapGrid CS.
    //! Example: if the global CS is placed in the center of your test site at a
    //! point with GPS coordinates (Xgps,Ygps) and the Y axis points North-West,
    //! then the GPS offset is (-Xgps, -Ygps, 45).
    Frame2d offset;

    //! Specifies location of the GPS antenna with respect to the vehicles's
    //! coordinate system. Note that the angles are meaningful only if D-GPS
    //! is used.
    Frame3d antennaOffset;
};

//! Gps position types
enum GpsPositionType {
    //! Invalid or not available
    GpsPositionTypeNotAvailable,
    //! Autonomous position
    //! (This is the normal case for non-differential GPS)
    GpsPositionTypeAutonomous,
    //! Differentially corrected
    GpsPositionTypeDifferential
};

//! Gps time structure
struct GpsTimeData
{
    //! Time (according to the computer clock) when data was measured.
    Time timeStamp;
    //! UTC time (according to GPS device), reference is Greenwich.
    TimeOfDay utcTime;
    //! UTC date (according to GPS device)
    Date utcDate;
};

//! Gps MapGrid data structure
struct GpsMapGridData
{
    //! Time (according to the computer clock) when data was measured.
    Time timeStamp;
    //! UTC time (according to GPS device), reference is Greenwich.
    TimeOfDay utcTime;

    //! Our current zone
    int zone;
    //! Northing (metres)
    double northing;
    //! Easting (metres)
    double easting;
    //! Altitude (metres above ellipsoid)
    double altitude;
    
    //! Horizontal position error: one standard deviation (metres)
    double horizontalPositionError;
    //! Vertical position error: one standard deviation (metres)
    double verticalPositionError;
    
    //! Heading/track/course with respect to true north (rad)
    double heading; 
    //! Horizontal velocity (metres/second)
    double speed;
    //! Vertical velocity (metres/second)
    double climbRate;
    
    //! Position type (see above)
    GpsPositionType positionType;
};


//! Gps data structure
struct GpsData
{
    //! Time (according to the computer clock) when data was measured.
    Time timeStamp;
    //! UTC time (according to GPS device), reference is Greenwich.
    TimeOfDay utcTime;
 
    //! Latitude (degrees)
    double latitude;
    //! Longitude (degrees)
    double longitude;
    //! Altitude (metres above ellipsoid)
    double altitude;
    
    //! Horizontal position error: one standard deviation (metres)
    double horizontalPositionError;
    //! Vertical position error: one standard deviation (metres)
    double verticalPositionError;
    
    //! Heading/track/course with respect to true north (rad)
    double heading; 
    //! Horizontal velocity (metres/second)
    double speed;
    //! Vertical velocity (metres/second)
    double climbRate;
    
    //! Number of satellites
    int satellites;
    //! Position type (see above)
    GpsPositionType positionType;
    //! Geoidal Separation (metres)
    double geoidalSeparation;    
};


/*!
 * Data consumer interface
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
    //! Returns the latest data
    nonmutating GpsData getData()
            throws HardwareFailedException;

    //! Return the gps description
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
};

interface GpsMapGrid
{
    //! Return the latest map information
    nonmutating GpsMapGridData getData()
            throws HardwareFailedException;

    //! Return the gps description
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
    void subscribe( GpsMapGridConsumer* subscriber )
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
    idempotent void unsubscribe( GpsMapGridConsumer* subscriber );
};

interface GpsTime
{
    //! Return the latest timestamp information
    nonmutating GpsTimeData getData()
            throws HardwareFailedException;

    //! Return the gps description
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
    void subscribe( GpsTimeConsumer* subscriber )
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
    idempotent void unsubscribe( GpsTimeConsumer* subscriber );
};


//!  //@}
}; // module

#endif
