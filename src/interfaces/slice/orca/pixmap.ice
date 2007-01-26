/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_PIXMAP_ICE
#define ORCA_PIXMAP_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{

/*!
    @ingroup  interfaces
    @defgroup orca_interface_pixmap PixMap
    @brief    An planar pixel representation of the world.
    
    PixMap is a pixel-based map which can be used as a background map for a GUI for instance.
    The data is encoded as pixels with rgb values.
  
    @see @ref orca_interface_ogmap for coordinate system conventions.
*/

//! Represents a single pixel encoded as rgb
struct Pixel
{
    byte r;
    byte g;
    byte b;    
};

//! A sequence of pixels representing an image
sequence<Pixel> Pixels;

struct PixMapData
{
    //! Time when data was measured.
    Time timeStamp;

    //! The global coordinates of the bottom-left corner of the bottom-left cell
    Frame2d      origin;
    //! The number of cells along the x axis
    int numCellsX;
    //! The number of cells along the y axis
    int numCellsY;
    //! The size of each cell, in the x dimension
    float metresPerCellX;
    //! The size of each cell, in the y dimension
    float metresPerCellY;

    //! The rgb pixel values
    Pixels rgbPixels; 
};


/*!
 * Data consumer interface
 *
 * Implement this if you need to subscribe for pixmap updates
 */
interface PixMapConsumer
{
    //! Transmits the data to the consumer.
    void setData( PixMapData obj );
};

//! Access to pixMap.
interface PixMap
{
    //! Returns the latest data.
    nonmutating PixMapData getData()
            throws DataNotExistException;

    /*!
     * Mimics IceStorm's subscribe(). @p subscriber is typically a direct proxy to the consumer object.
     * The implementation may choose to implement the push directly or use IceStorm.
     * This choice is transparent to the subscriber. The case when the @p subscriber is already subscribed
     * is quietly ignored.
     *
     * @see unsubscribe
     */
    void subscribe( PixMapConsumer *subscriber )
            throws SubscriptionFailedException;
    
    /*!
     * Unsubscribe an existing @p subscriber. The case when the @p subscriber is not subscribed
     * is quietly ignored.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( PixMapConsumer *subscriber );
}; 

}; // module

/*! @} */
#endif
