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

#ifndef ORCA2_STATUS_INTERFACE_ICE
#define ORCA2_STATUS_INTERFACE_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_status Status
    @brief Access to a component's health and status subsystem
    @{
*/

/*!
    @brief Component status data.
*/
class StatusData extends OrcaObject
{
    //! The fully-qualified name of the interface.
    FQComponentName name;
    //! Message category, e.g. warning, error, etc.
    string category;
    //! Verbosity level, 1 being the lowest
    int verbosity;
    //! Status message from component.
    string message;
};


/*!
 *
 * Data consumer interface (needed only for the push pattern).
 *
 * In Orca-1 terms, this the Consumer side of the ClientPush interface.
 *
 */
interface StatusConsumer
{
    //! ServerPush_Consumer interface
    void setData( StatusData obj );
};

/*!
 *  @brief Access to a robot's status subsystem
 */
interface Status
{
    //! Returns the latest status.
    nonmutating StatusData getData();

    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @see unsubscribe
     */
    void subscribe( StatusConsumer* subscriber );
    
    // for reference, this is what IceStorm's subscribe function looks like.
    //void subscribe(QoS theQoS, Object* subscriber);
    
    /*!
     * Unsubscribe the given [subscriber].
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( StatusConsumer* subscriber );
};


/*!
    @}
*/
}; // module

#endif
