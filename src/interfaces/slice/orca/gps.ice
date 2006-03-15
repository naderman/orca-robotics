/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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

//! Gps time structure - for time sync
class GpsTime extends OrcaObject
{
    // UTC time
    //! Hours
    int utcHours;
    //! minutes
    int utcMinutes;
    //! seconds
    double utcSeconds;
    //! day
    int day;
    //! month
    int month;
    //! year
    int year;
};

//! Gps data structure
class GpsData extends OrcaObject
{
    // UTC time
    //! Hours
    int utcHours;
    //! minutes
    int utcMinutes;
    //! seconds
    double utcSeconds;
 
    // GPS position
    //! Latitude (Degrees)
    double latitude;
    //! Longitude (Degrees)
    double longitude;
    //! Altitude (Metres above Ellipsoid)
    double altitude;
    
    // Velocities
    // heading/track/course (Degrees)
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
    @brief Access to GPS data.
*/
interface Gps
{
    //! Returns the latest data.
    //! @note In Orca1 this would be called ClientPull_Supplier interface.
    nonmutating GpsData getData()
            throws HardwareFailedException;

    //! Return the latest timestamp information
    nonmutating GpsTime getTime()
            throws HardwareFailedException;


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
