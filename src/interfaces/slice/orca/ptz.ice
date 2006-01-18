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

#ifndef ORCA2_PTZ_INTERFACE_ICE
#define ORCA2_PTZ_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_ptz Ptz
    @brief Pan-tilt-zoom unit

    @{
*/

class PtzConfig extends OrcaObject {
    float minPanAngle;
    float maxPanAngle; 
    float minTiltAngle;
    float maxTiltAngle; 
    float minZoom; 
    float maxZoom; 	  
};


//! PTZ geometry data structure
class PtzGeometry extends OrcaObject
{
    //! Offset of the centre of the laser from the robot, int the robot CS
    Frame3d offset;

    //! Dimensions of the laser
    Size3d  size;
};

/*!
    Ptz data structure
*/
class PtzData extends OrcaObject
{
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
    nonmutating PtzData      getData();
    nonmutating PtzConfig    getConfig();
    nonmutating PtzGeometry  getGeometry();

    /*!
     *
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the push or use IceStorm. This choice
     * is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @param preferedPushInterval The subscriber's preference for how often it wants to
     * receive updates [sec]. Provider's ability to fulfil this request may vary.
     *
     * @see unsubscribe
     *
     */
    void subscribe( PtzConsumer *subscriber );

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
