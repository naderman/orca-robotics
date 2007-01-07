/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, George Mathews
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCAOGMAP_OGMAPUTILS_H
#define ORCAOGMAP_OGMAPUTILS_H

#include <assert.h>
#include <cmath>
#include <vector>
#include <string>

namespace orcaogmap {

    //! CartesianPoint2d and Frame2d are defined here to be independent from any Ice-specific representation. They use the same names as in @ref orca
    struct CartesianPoint2d
    {
        //! x-coordinate [m]
        float x;
        //! y-coordinate [m]
        float y;
    };
    std::string toString(const CartesianPoint2d &p);

    struct Frame2d
    {
        //! Point of origin
        CartesianPoint2d p;
        //! Orientation angle [rad]
        float o;
    };
    std::string toString(const Frame2d &f);

    //! Signifies absolute certainty that the cell is empty
    //! @relatesalso OgMap
    const int CELL_EMPTY = 0;

    //! Signifies no knowledge of cell occupancy
    //! @relatesalso OgMap
    const int CELL_UNKNOWN = 127;

    //! Signifies absolute certainty that the cell is occupied
    //! Avoid 255 so that there's a definite midpoint.
    //! @relatesalso OgMap
    const int CELL_OCCUPIED = 254;

    //! Components' internal storage of an OgMap (no Ice dependencies). The member variables have the same names as defined in bros1.ice.
    class OgMap
    {
    public:

        // member access functions
        
        //! Returns the number of cells along the x-axis 
        int numCellsX() const { return numCellsX_; };

        //! Returns the number of cells along the y-axis
        int numCellsY() const { return numCellsY_; };

        //! Returns the size of the world, in metres, along the x-axis
        float worldSizeX() const { return numCellsX_*metresPerCellX_; };
        //! Returns the size of the world, in metres, along the y-axis
        float worldSizeY() const { return numCellsY_*metresPerCellY_; };

        //! Set the origin (global coordinates of the bottom-left corner of the bottom-left cell)
        Frame2d & origin() { return origin_; };
        //! Get the origin (global coordinates of the bottom-left corner of the bottom-left cell)
        const Frame2d & origin() const { return origin_; };
        
        //! Set the size of each cell, in the x dimension
        void setMetresPerCellX( float mx ) { metresPerCellX_ = mx; };
        //! Get the size of each cell, in the x dimension
        float metresPerCellX() const { return metresPerCellX_; };
        
        //! Set the size of each cell, in the y dimension
        void setMetresPerCellY( float my ) { metresPerCellY_ = my; };
        //! Get the size of each cell, in the y dimension
        float metresPerCellY() const { return metresPerCellY_; };

        //! Set ogmap occupancy values
        unsigned char* data() { return &data_[0]; };
        //! Get ogmap occupancy values
        const unsigned char* data() const { return &data_[0]; };

        //! Get the x world coord of a grid cell.
        //! (Returns the coords of the centre of the cell)
        //! gridX doesn't have to be within the map bounds.
        float worldXCoord( int gridX ) const;
        //! Get the y world coord of a grid cell.
        //! (Returns the coords of the centre of the cell)
        //! gridY doesn't have to be within the map bounds.
        float worldYCoord( int gridX ) const;

        //! Get the world coordinates of a grid cell.
        //! (Returns the coords of the centre of the cell)
        //! gridX and gridY don't have to be within the map bounds.
        void getWorldCoords( int gridX, int gridY, float &worldX, float &worldY ) const;
    
        //! Get the grid cell indices of a point in the world
        //! worldX and worldY don't have to be within the map bounds.
        void getCellIndices( float worldX, float worldY, int &gridX, int &gridY ) const;

        //! Set a cell indexed by map coords
        //! (no bounds checking is performed)
        unsigned char & gridCell( int indX, int indY )
        {
            return data_[indY*numCellsX() + indX];
        }        
        //! Get a cell, indexed by map coords
        //! (no bounds checking is performed)
        const unsigned char & gridCell( int indX, int indY ) const
        {
            return data_[indY*numCellsX() + indX];
        }
    
        //! Set a cell indexed by world coords 
        //! (no bounds checking is performed)       
        unsigned char &worldCell( float worldX, float worldY )
        { 
            assert( origin_.o == 0.0 );
            return gridCell( worldToIndexX(worldX), worldToIndexY(worldY) ); 
        }
        //! Get a cell, indexed by world coords
        //! (no bounds checking is performed)
        const unsigned char &worldCell( float worldX, float worldY ) const
        { 
            assert( origin_.o == 0.0 );
            return gridCell( worldToIndexX(worldX), worldToIndexY(worldY) ); 
        }

        //! Tries to get a cell (bounds check performed). Returns TRUE if successful, FALSE if the cell is outside the map.
        bool tryGridCell( int indX, int indY, unsigned char & cell ) const;
    
        //! Tries to get a cell (bounds check performed). Returns TRUE if successful, FALSE if the cell is outside the map.
        bool tryWorldCell( float worldX, float worldY, unsigned char & cell ) const;
           
        //! translate a world coord to a map coord
        int worldToIndexX( float worldX ) const
        { assert(origin_.o==0.0); return (int) std::floor( (worldX-origin_.p.x)/metresPerCellX_ ); }
        int worldToIndexY( float worldY ) const
        { assert(origin_.o==0.0); return (int) std::floor( (worldY-origin_.p.y)/metresPerCellY_ ); }
    
        //! Change the size of the map
        void reallocate( int numCellsX, int numCellsY );
    
        //! Fill the map with the given value
        void fill( unsigned char cellValue );
    
        //! Are the world coordinates within the map?
        bool coordsWithinMap( float worldX, float worldY ) const
        { 
            assert( origin_.o == 0.0 );
            return ( worldX >= origin_.p.x &&
                     worldY >= origin_.p.y &&
                     worldX <  origin_.p.x+numCellsX()*metresPerCellX() &&
                     worldY <  origin_.p.y+numCellsY()*metresPerCellY() );
        }

        //! Is the grid cell within the map?
        bool cellWithinMap( float gridX, float gridY ) const
        { 
            return ( gridX >= 0 &&
                     gridY >= 0 &&
                     gridX <  numCellsX() &&
                     gridY <  numCellsY() );
        }

    private:

        //! The global coordinates of the bottom-left corner of the bottom-left cell
        Frame2d origin_;
    
        //! The size of each cell, in the x dimension
        float metresPerCellX_;
        //! The size of each cell, in the y dimension
        float metresPerCellY_;
    
        //! The occupancy values.  To give a definite mid-point the last level (255) is not used:
        //!   i.e. 0 = unoccupied, 254 = occupied and 127 = unknown. 
        std::vector<unsigned char> data_;
    
        //! The number of cells along the x axis
        int numCellsX_;
        //! The number of cells along the y axis
        int numCellsY_;
    };

    //! Fuses the map tile 'update' into the master map, by replacing the values in
    //! 'master' with the values in 'update'.
    //! The master will be resized if necessary.  Any new cells whose
    //! occupancy cannot be determined by the update are set to 'newCells'.
    //! 
    //! Assumes: 
    //! - the resolutions match
    //! - the maps have zero orientation.
    void fuse( OgMap &master, const OgMap &update, unsigned char newCells=127 ); 
    
    //! Overlays two OgMaps, i.e. their occupancy values are bitwise or'd. 
    //! Resulting map is stored in 'master'.
    //! Assumes that all map parameters (origin, resolution, number of cells) are the same.
    void overlay( OgMap &master, const OgMap &slave ); 

    //! Convert an occupancy grid map to text
    std::string toText( const OgMap &map );

    //! Returns a character corresponding to occupancy (empty:' '; occupied:'#'; in-between:'-') 
    char displayOgmapCell(unsigned char cell);
 
}

std::ostream &operator<<( std::ostream &s, const orcaogmap::OgMap &o );
 
#endif
