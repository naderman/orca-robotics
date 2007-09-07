#ifndef ORCAOGMAP_GENERICMAP_H
#define ORCAOGMAP_GENERICMAP_H

#include <assert.h>
#include <cmath>
#include <vector>
#include <string>
#include <ostream>

namespace orcaogmap {

    //! CartesianPoint2d and Frame2d are defined here to be independent 
    //! from any Ice-specific representation. They use the same names as in @ref orca
    struct CartesianPoint2d
    {
        //! x-coordinate [m]
        double x;
        //! y-coordinate [m]
        double y;
    };
    std::string toString(const CartesianPoint2d &p);

    struct Frame2d
    {
        //! Point of offset
        CartesianPoint2d p;
        //! Orientation angle [rad]
        double o;
    };
    std::string toString(const Frame2d &f);

    //! A (spatial) map of values: posit a 2D grid of cells over the world, 
    //! with a value stored at each cell.
    template<typename T>
    class GenericMap
    {
    public:

        GenericMap()
            {
                reallocate(0,0);
                metresPerCellX_ = 0;
                metresPerCellY_ = 0;
                offset_.p.x = 0;
                offset_.p.y = 0;
                offset_.o = 0;
            }

        GenericMap( int numCellsX, 
                    int numCellsY, 
                    double offsetX,
                    double offsetY,
                    double offsetTheta,
                    double metresPerCellX,
                    double metresPerCellY,
                    const T &initialValue )
            {
                reallocate( numCellsX, numCellsY );
                metresPerCellX_ = metresPerCellX;
                metresPerCellY_ = metresPerCellY;
                offset_.p.x = offsetX;
                offset_.p.y = offsetY;
                offset_.o = offsetTheta;
                fill( initialValue );
            }

        GenericMap( int numCellsX, 
                    int numCellsY, 
                    const Frame2d &offset,
                    double metresPerCellX,
                    double metresPerCellY,
                    const T &initialValue )
            {
                reallocate( numCellsX, numCellsY );
                metresPerCellX_ = metresPerCellX;
                metresPerCellY_ = metresPerCellY;
                offset_ = offset;
                fill( initialValue );
            }

        // member access functions
        
        //! Returns the number of cells along the x-axis 
        int numCellsX() const { return numCellsX_; };

        //! Returns the number of cells along the y-axis
        int numCellsY() const { return numCellsY_; };

        //! Returns the size of the world, in metres, along the x-axis
        double worldSizeX() const { return numCellsX_*metresPerCellX_; };
        //! Returns the size of the world, in metres, along the y-axis
        double worldSizeY() const { return numCellsY_*metresPerCellY_; };

        //! Set the offset (global coordinates of the bottom-left corner of the bottom-left cell)
        Frame2d &offset() { return offset_; };
        //! Get the offset (global coordinates of the bottom-left corner of the bottom-left cell)
        const Frame2d &offset() const { return offset_; };
        
        //! Set the size of each cell, in the x dimension
        void setMetresPerCellX( double mx ) { metresPerCellX_ = mx; };
        //! Get the size of each cell, in the x dimension
        double metresPerCellX() const { return metresPerCellX_; };
        
        //! Set the size of each cell, in the y dimension
        void setMetresPerCellY( double my ) { metresPerCellY_ = my; };
        //! Get the size of each cell, in the y dimension
        double metresPerCellY() const { return metresPerCellY_; };

        //! Set values
        T* data() { return &data_[0]; };
        //! Get values
        const T* data() const { return &(data_[0]); };

        //! Get the x world coord of a grid cell.
        //! (Returns the coords of the centre of the cell)
        //! gridX doesn't have to be within the map bounds.
        double worldXCoord( int gridX ) const
            { return double(offset_.p.x + (gridX+0.5)*metresPerCellX_); }
        //! Get the y world coord of a grid cell.
        //! (Returns the coords of the centre of the cell)
        //! gridY doesn't have to be within the map bounds.
        double worldYCoord( int gridY ) const
            { return double(offset_.p.y + (gridY+0.5)*metresPerCellY_); }

        //! Get the world coordinates of a grid cell.
        //! (Returns the coords of the centre of the cell)
        //! gridX and gridY don't have to be within the map bounds.
        void getWorldCoords( int gridX, int gridY, double &worldX, double &worldY ) const
            {
                // Todo: handle non-zero orientation.
                assert(offset_.o == 0.0 );

                worldX = worldXCoord( gridX );
                worldY = worldYCoord( gridY );
            }

        //! Get the grid cell indices of a point in the world
        //! worldX and worldY don't have to be within the map bounds.
        void getCellIndices( double worldX, double worldY, int &gridX, int &gridY ) const
            {
                gridX = worldToIndexX( worldX );
                gridY = worldToIndexY( worldY );
            }

        //! Set a cell indexed by map coords
        //! (no bounds checking is performed)
        T &gridCell( int indX, int indY )
        {
            return data_[indY*numCellsX() + indX];
        }        
        //! Get a cell, indexed by map coords
        //! (no bounds checking is performed)
        const T &gridCell( int indX, int indY ) const
        {
            return data_[indY*numCellsX() + indX];
        }
    
        //! Set a cell indexed by world coords 
        //! (no bounds checking is performed)       
        T &worldCell( double worldX, double worldY )
        { 
            assert( offset_.o == 0.0 );
            return gridCell( worldToIndexX(worldX), worldToIndexY(worldY) ); 
        }
        //! Get a cell, indexed by world coords
        //! (no bounds checking is performed)
        const T &worldCell( double worldX, double worldY ) const
        { 
            assert( offset_.o == 0.0 );
            return gridCell( worldToIndexX(worldX), worldToIndexY(worldY) ); 
        }

        //! Tries to get a cell (bounds check performed). Returns TRUE if successful, FALSE if the cell is outside the map.
        bool tryGridCell( int indX, int indY, T &cell ) const
            {
                double worldX, worldY;
                getWorldCoords( indX, indY, worldX, worldY );
                return tryWorldCell( worldX, worldY, cell);
            }

        //! Tries to get a cell (bounds check performed). Returns TRUE if successful, FALSE if the cell is outside the map.
        bool tryWorldCell( double worldX, double worldY, T &cell ) const
            {
                if ( coordsWithinMap( worldX, worldY ) == true )
                {
                    cell = worldCell( worldX, worldY );
                    return true;
                }
                return false;
            }

        //! translate a world coord to a map coord
        int worldToIndexX( double worldX ) const
        { assert(offset_.o==0.0); return (int) std::floor( (worldX-offset_.p.x)/metresPerCellX_ ); }
        int worldToIndexY( double worldY ) const
        { assert(offset_.o==0.0); return (int) std::floor( (worldY-offset_.p.y)/metresPerCellY_ ); }
    
        //! Change the size of the map
        void reallocate( int numCellsX, int numCellsY )
            {
                numCellsX_ = numCellsX;
                numCellsY_ = numCellsY;
                data_.resize( numCellsX_ * numCellsY_ );
            }

        //! Fill the map with the given value
        void fill( const T &cellValue )
            {
                std::fill( data_.begin(), data_.end(), cellValue );
            }
                
    
        //! Are the world coordinates within the map?
        bool coordsWithinMap( double worldX, double worldY ) const
        { 
            assert( offset_.o == 0.0 );
            return ( worldX >= offset_.p.x &&
                     worldY >= offset_.p.y &&
                     worldX <  offset_.p.x+numCellsX()*metresPerCellX() &&
                     worldY <  offset_.p.y+numCellsY()*metresPerCellY() );
        }

        //! Is the grid cell within the map?
        bool cellWithinMap( double gridX, double gridY ) const
        { 
            return ( gridX >= 0 &&
                     gridY >= 0 &&
                     gridX <  numCellsX() &&
                     gridY <  numCellsY() );
        }

    private:

        //! The global coordinates of the bottom-left corner of the bottom-left cell
        Frame2d offset_;
    
        //! The size of each cell, in the x dimension
        double metresPerCellX_;
        //! The size of each cell, in the y dimension
        double metresPerCellY_;
    
        //! The values
        std::vector<T> data_;
    
        //! The number of cells along the x axis
        int numCellsX_;
        //! The number of cells along the y axis
        int numCellsY_;
    };

    template<typename T>
    std::ostream &operator<<( std::ostream &s, const GenericMap<T> &o )
    {
        s << " OgMap: \n"
          << "\toffset:        [" << o.offset().p.x << ", " << o.offset().p.y << ", " << o.offset().o*180.0/M_PI << "]\n"
          << "\tnumCells:      [" << o.numCellsX() << ", " << o.numCellsY() << "]\n"
          << "\tmetresPerCell: [" << o.metresPerCellX() << ", " << o.metresPerCellY() << "]\n";

        return s;
    }
 
}

#endif
