/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PTZ_INTERFACE_ICE
#define ORCA2_PTZ_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_ptz Ptz
    @brief Pan-tilt-zoom unit

    @{
*/

struct PtzConfig
{
    //! Time when data was measured.
    Time timeStamp;
    float minPanAngle;
    float maxPanAngle; 
    float minTiltAngle;
    float maxTiltAngle; 
    float minZoom; 
    float maxZoom; 	  
};


//! PTZ geometry data structure
struct PtzGeometry
{
    //! Time when data was measured.
    Time timeStamp;

    //! Offset of the centre of the laser from the robot, int the robot CS
    Frame3d offset;

    //! Dimensions of the laser
    Size3d  size;
};

/*!
    Ptz data structure
*/
struct PtzData
{
    //! Time when data was measured.
    Time timeStamp;
    float pan; // From straight ahead 
    float tilt; // From flat
    float zoom; // Expressed as the field of view. 
};

/*!
    @brief Consumer of ptz updates
*/
interface PtzConsumer
{
    void setData( PtzData obj );
};

/*!
    @brief Single-origin scanning range sensor
 */
interface Ptz
{
    //! Functions for reading from the laser
    ["cpp:const"] idempotent PtzData      getData();
    ["cpp:const"] idempotent PtzConfig    getConfig();
    ["cpp:const"] idempotent PtzGeometry  getGeometry();

    /*!
     *
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the push or use IceStorm. This choice
     * is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * possible additional parameter:
     * preferedPushInterval The subscriber's preference for how often it wants to
     * receive updates [sec]. Provider's ability to fulfil this request may vary.
     *
     * @see unsubscribe
     *
     */
    void subscribe( PtzConsumer *subscriber )
            throws SubscriptionFailedException;

    // this is what IceStorm's subscribe function looks like.
    //void subscribe(QoS theQoS, Object* subscriber);

    /**
     *
     * Unsubscribe the given [subscriber].
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     *
    **/
    idempotent void unsubscribe( PtzConsumer *subscriber );
};

/*! @} */
}; // module

#endif
