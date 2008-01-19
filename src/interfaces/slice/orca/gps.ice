/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
    @ingroup orca_interfaces
    @defgroup orca_interface_gps Gps
    @brief Access to GPS devices.
    @{
*/


//! Gps config structure
struct GpsDescription
{
    //! Time (according to the computer clock) when data was measured.
    Time timeStamp;
    
    //! Specifies location of the GPS antenna with respect to the vehicles's
    //! coordinate system. Note that the angles are meaningful only if D-GPS
    //! is used.
    Frame3d antennaOffset;
};

//! Gps position types.
//! Using Novatel codes here is probably not the best thing. With more
//! thought it's probably possible to categorize these position types
//! into more generic categories. For now, non-Novatel receivers should
//! use the generic types listed first.
enum GpsPositionType {
    //! Invalid or not available
    GpsPositionTypeNotAvailable,
    //! Autonomous position
    //! (This is the normal case for non-differential GPS)
    GpsPositionTypeAutonomous,
    //! Differentially corrected
    GpsPositionTypeDifferential,
    NovatelNone,
    NovatelFixedPos,
    NovatelFixedHeigth,
    NovatelFloatConv,
    NovatelWideLane,
    NovatelNarrowLane,
    NovatelDopplerVelocity,
    NovatelSingle,
    NovatelPsrDiff,
    NovatelWAAS,
    NovatelPropagated,
    NovatelOmnistar,
    NovatelL1Float,
    NovatelIonFreeFloat,
    NovatelNarrowFloat,
    NovatelL1Int,
    NovatelWideInt,
    NovatelNarrowInt,
    NovatelRTKDirectINS,
    NovatelINS,
    NovatelINSPSRSP,
    NovatelINSPSRFLOAT,
    NovatelINSRTKFLOAT,
    NovatelINSRTKFIXED,
    NovatelOmnistarHP,
    NovatelUnknown
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
    int observationCountOnL1;
    int observationCountOnL2;
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
    @brief Access to GpsData (raw data)
*/
interface Gps
{
    //! Returns the latest data
    ["cpp:const"] idempotent GpsData getData()
            throws HardwareFailedException;

    //! Return the gps description
    ["cpp:const"] idempotent GpsDescription getDescription();

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

//!  //@}
}; // module

#endif
