/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Duncan Mercer
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ESTOP_ICE
#define ORCA_ESTOP_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_estop EStop
    @brief Interface that provides a 'keep alive' pulse on an interface.
   

The EStop interface provides a mechanism for indicating to a system
that all is well at some higher level. Regular data is expected across
the interface. Missing data or data indicating an error state should prompt
the consumer to 'fail-safe' in the most appropriate manner.

*/


struct EStopData
{
    //! Time when data was generated.
    Time timeStamp;
    //! 
    bool hasFaultCondition;
};



interface EStopConsumer
{
    //! Sends data to the consumer.
    void setData(EStopData state);
};


//! Interface to a device with a binary state.
interface EStop
{
    //! Returns the latest data. Raises DataNotExistException if data is not available.
    ["cpp:const"] idempotent EStopData getData()
        throws DataNotExistException, HardwareFailedException;

    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @see unsubscribe
     */
    void subscribe( EStopConsumer* subscriber )
            throws SubscriptionFailedException;

    /*!
     * Unsubscribe the given [subscriber].
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( EStopConsumer* subscriber );


};

/*! @} */

}; // module

#endif
