/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_BINARY_SWITCH_ICE
#define ORCA_BINARY_SWITCH_ICE

#include <orca/common.ice>
#include <IceStorm/IceStorm.ice>

module orca
{
    
/*!
    @ingroup  interfaces
    @defgroup orca_interface_binaryswitch BinarySwitch
    @brief    An simple on-off switch

    @{
*/

//! Binary Switch data structure
struct BinarySwitchData
{
    //! on/off = true/false
    bool switchValue;
};

/*!
 * Data consumer interface
 *
 * Implement this if you need to subscribe for updates
 */
interface BinarySwitchConsumer
{
    //! Transmits the data to the consumer.
    void setData( BinarySwitchData obj );
};

//! Access to the switch.
interface BinarySwitch
{
    //! Returns the latest data.
    idempotent BinarySwitchData getData()
            throws DataNotExistException;

    //! Sets the switch's state
    void setData( BinarySwitchData data );

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( BinarySwitchConsumer* subscriber )
        throws SubscriptionFailedException, SubscriptionPushFailedException;
}; 

}; // module

/*! @} */
#endif
