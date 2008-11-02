/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_QGRAPHICS_2D_INTERFACE_ICE
#define ORCA2_QGRAPHICS_2D_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <orca/stochastic.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_qgraphics2d QGraphics2d
    @brief Components can implement this interface to define how they'd like
           to draw themselves.

           The interface is Qt-specific.  It works like this:

             1) The interface provider instantiates a QPicture, and paints to it.
           
             2) The QPicture knows how to serialize itself.  The interface provider
                serializes the QPicture into the QGraphics2dData object.

             3) A component that knows how to paint things connects and gets the
                QGraphics2dData object, de-serializes it into a QPicture, and paints it.

           This allows extremely general painting: the interface provider can use all
           of Qt's power to paint into the QPicture.

    @par Example

        An example of how an interface provider would draw itself:

        @verbatim
        //
        // Set up the QPicture
        //
        QPicture qpic;
        QPainter p;
        p.begin( &qpic );
        {
            //
            // Painting code goes here: paint to the painter...
            //
        }
        p.end();

        //
        // Then stick it in the orca object
        //
        orca::QGraphics2dDataPtr data = new QGraphics2dData;

        data->z = [1..12];
        data->isInGlobalCS = {true|false};

        data->qpicture.resize( qpic.size() );
        memcpy( &(data->qpicture[0]), qpic.data(), qpic.size() );
        @endverbatim

    @{
*/

//! A serialized QPicture
// sequence<byte> SerializedQPicture;

//! QGraphics2d command structure: tells how to paint something.
struct QGraphics2dData
{
    //! Time when data was measured.
    Time timeStamp;

    //! The serialized QPicture
    ByteSeq qpicture;

    //!
    //! The level at which the qpicture should be painted.
    //! valid values are 1-12 inclusive.
    //! 1 is the background, 12 is the foreground.
    //!
    int z;

    //! If true, the painting should be done in the global coordinate system.
    //!
    //! If false, the painting is intended to be done in the platform's
    //! local coordinate system.
    //!
    bool isInGlobalCS;
};

/*!
 *   Data consumer interface.
 *   A component should implement this interface if it's capable of drawing stuff.
 */
interface QGraphics2dConsumer
{
   //! Transmits the data to the consumer.
   void setData( QGraphics2dData obj );
};

/*!
    @brief Access to the drawable entity
*/
interface QGraphics2d
{
   //! Returns the latest data.
   idempotent QGraphics2dData getData()
            throws DataNotExistException;
   /*!
    * Mimics IceStorm's subscribe() but without QoS, for now. The
    * implementation may choose to implement the data push internally
    * or use IceStorm. This choice is transparent to the subscriber.
    *
    * @param subscriber The subscriber's proxy.
    *
    * @see unsubscribe
    */
   void subscribe( QGraphics2dConsumer* subscriber )
        throws SubscriptionFailedException;

    // for reference, this is what IceStorm's subscribe function looks like.
    //void subscribe(QoS theQoS, Object* subscriber);

    /*!
     * Unsubscribe the given @p subscriber.
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( QGraphics2dConsumer* subscriber );
};


//!  //@}
}; // module

#endif
