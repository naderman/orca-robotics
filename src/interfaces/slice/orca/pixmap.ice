/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_PIXMAP_ICE
#define ORCA_PIXMAP_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <IceStorm/IceStorm.ice>

module orca
{

/*!
    @ingroup  interfaces
    @defgroup orca_interface_pixmap PixMap
    @brief    An planar pixel representation of the world.
    
    PixMap is a pixel-based map which can be used as a background map for a GUI for instance.
    The data is encoded as pixels with rgb values.
  
    @see @ref orca_interface_ogmap for coordinate system conventions.
    @{
*/

//! Represents a single pixel encoded as Red-Green-Blue intensities.
struct Pixel
{
    //! Red intensity.
    byte r;
    //! Green intensity.
    byte g;
    //! Blue intensity.
    byte b;    
};

//! A sequence of pixels representing an image
sequence<Pixel> Pixels;

//! Data for PixMap interface
struct PixMapData
{
    //! Time when data was measured.
    Time timeStamp;

    //! Transformation from the global (arbitrarily defined) coordinate system 
    //! (CS) to the CS of the pixmap. The pixmap CS is 
    //! defined as as follows:
    //! - origin at the bottom-left corner
    //! - X axis along the bottom edge to the right
    //! - Y axis along the left edge up
    //! Example: if the global CS starts in the middle of a pixmap and there are
    //! no rotations, then the pixmap offset is (-width/2, -height/2, 0).
    Frame2d offset;
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
    idempotent PixMapData getData()
            throws DataNotExistException;

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( PixMapConsumer* subscriber )
        throws SubscriptionFailedException;
}; 

}; // module

/*! @} */
#endif
