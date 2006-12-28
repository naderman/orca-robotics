/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_OGMAP_ICE
#define ORCA_OGMAP_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
    
module ogmaptype
{
    //! Map represents occupancy
    const int OCCUPANCY = 0;
    //! Map represents hazards
    const int HAZARD    = 1;
    //! Map represents motion
    const int MOTION    = 2;    
};

/*!
    @ingroup  interfaces
    @defgroup orca_interface_ogmap OgMap
    @brief    An planar occupancy-grid representation of the world

The OgMapData object is a representation of a part of the world.
It need not represent the entire world.  By describing only a subset of
the world, agents can transmit updates to one another, which can
be fused to create a larger occupancy-grid map.

@see @ref orca_library_orcaogmap
    @{
*/

/*!
  Occupancy-grid representation of a portion of the world.

  There are conceptually two coordinate systems:

  1) The global continuous coordinate system.
  2) The discrete grid-based coordinate system.

  In addition:

  3) The array of cells can be indexed by a single integer.

  The three methods of referring to cells are defined as follows:

      <pre>
     1) GLOBAL COORDINATE SYSTEM              2) CELL INDEXING                 3) DATA STORAGE
         worldCell(double,double)              gridCell(int,int)            e.g. OgMapData->data(index)
           _________________                   _________________                _________________
          |                 |     numCellsY-1 |  ^              |              |                 | 
 world    |                 |         ^       |  |              |              |                 | 
 y-axis   |                 |         .       |  |              |              |                 |
   ^      |                 |         . indY  |  |sizeY         |              |                 |
   |      |                 |         .       |  |              |              |                 |
   |----->|                 |         .       |  |    sizeY     |              |----> index      |
   |  oX  |                 |         .       |<-|------------->|              |---------------->|
   |      |_________________|         0       |__v______________|              |_________________|
   |         ^                                
   |         | oY     world                           indX                   index: 0 -> sizeX * sizeY - 1
   |_________|_____>  x-axis                  0 - - - - - - -> numCellsX-1

   (oX = origin.p.x, oY = origin.p.y)

   (note: in (1), the map is aligned with the world axes.  This need not be the case.)
   </pre>

   For OgMapData manipulation functions, see @ref orca_library_orcaogmap
*/
struct OgMapData
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

    //! The occupancy values.  To give a definite mid-point the last level (255) is not used:
    //!   i.e. 0 = unoccupied, 254 = occupied and 127 = unknown. 
    ByteSequence data;
    
    //! User-defined type of certainty grid map.
    //! Useful to distinguish between occupancy, motion, hazard maps etc.
    int mapType;
};

module ogmapvalues
{
    //! The midpoint between occupied and empty
    const int UNKNOWN = 127;
};

/*!
 * Data consumer interface
 *
 * Implement this if you need to subscribe for map updates
 */
interface OgMapConsumer
{
    //! Transmits the data to the consumer.
    void setData( OgMapData obj );
};

//! Access to occupancy-grid map.
interface OgMap
{
    //! Returns the latest data.
    nonmutating OgMapData getData()
            throws DataNotExistException;

    /*!
     * Mimics IceStorm's subscribe(). @p subscriber is typically a direct proxy to the consumer object.
     * The implementation may choose to implement the push directly or use IceStorm.
     * This choice is transparent to the subscriber. The case when the @p subscriber is already subscribed
     * is quietly ignored.
     *
     * @see unsubscribe
     */
    void subscribe( OgMapConsumer *subscriber )
            throws SubscriptionFailedException;
    
    /*!
     * Unsubscribe an existing @p subscriber. The case when the @p subscriber is not subscribed
     * is quietly ignored.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( OgMapConsumer *subscriber );
}; 

}; // module

/*! @} */
#endif
