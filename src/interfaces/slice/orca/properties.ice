/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROPERTIES_ICE
#define ORCA2_PROPERTIES_ICE

#include <orca/common.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_properties Properties
    @brief Centralised Properties server

    @{
*/

/*!
    @brief Information about properties
*/
struct PropertiesData
{
    //! The set of properties
    StringStringDict properties;
};

/*!
    @brief Subscriber to Properties information.
*/
interface PropertiesConsumer
{
    //! Set data
    void setData( PropertiesData data );
};

//! Interface to Properties information.
interface Properties
{
    //! Get current Properties state. Raises DataNotExistException if data is not available.
    idempotent PropertiesData getData()
        throws DataNotExistException;

    //! Allow remote clients to set new properties.  
    //! These will be merged with the working property set:
    //!   - new properties are added to the set.
    //!   - properties which already exist in the set are over-written.
    void setData( PropertiesData data );

    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @see unsubscribe
     */
    void subscribe( PropertiesConsumer* subscriber )
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
    idempotent void unsubscribe( PropertiesConsumer* subscriber );
};

/*! @} */
}; // module

#endif
