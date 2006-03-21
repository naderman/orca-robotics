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

#ifndef ORCA2_QGRAPHICS_2D_INTERFACE_ICE
#define ORCA2_QGRAPHICS_2D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>
#include <orca/stochastic.ice>
#include <orca/position2d.ice>

module orca
{
/*!
    @ingroup interfaces
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
sequence<byte> SerializedQPicture;

//! QGraphics2d command structure: tells how to paint something.
class QGraphics2dData extends OrcaObject
{
    //! The serialized QPicture
    SerializedQPicture qpicture;

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
   nonmutating QGraphics2dData getData();

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
