/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROPERTIES_ICE
#define ORCA2_PROPERTIES_ICE

#include <orca/common.ice>
#include <IceStorm/IceStorm.ice>

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

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( PropertiesConsumer* subscriber )
        throws SubscriptionFailedException;
};

/*! @} */
}; // module

#endif
