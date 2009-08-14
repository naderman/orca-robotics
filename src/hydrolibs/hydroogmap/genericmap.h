#ifndef HYDROOGMAP_GENERICMAP_H
#define HYDROOGMAP_GENERICMAP_H

#include <assert.h>
#include <cmath>
#include <vector>
#include <string>
#include <ostream>
#include <hydroportability/sharedlib.h>

namespace hydroogmap {

    //! A point in the (continuous) world
    struct WorldCoords {
        WorldCoords( double worldX, double worldY )
            : x(worldX), y(worldY) {}
        WorldCoords() {}
        double x;
        double y;
    };
    SOEXPORT std::string toString(const WorldCoords &w);
    inline std::ostream &operator<<( std::ostream &s, const WorldCoords &w )
    { return s << toString(w); }

    //! A cell in the grid
    struct GridIndices {
        GridIndices( int gridX, int gridY )
            : x(gridX), y(gridY) {}
        GridIndices() {}
        int x;
        int y;
    };
    SOEXPORT std::string toString(const GridIndices &g );
    inline std::ostream &operator<<( std::ostream &s, const GridIndices &g )
    { return s << toString(g); }

    struct Frame2d
    {
        Frame2d( const WorldCoords &offsetP, double offsetO )
            : p(offsetP), o(offsetO) {}
        Frame2d( double offsetX, double offsetY, double offsetO )
            : p(offsetX,offsetY), o(offsetO) {}

        //! Point of offset
        WorldCoords p;
        //! Orientation angle [rad]
        double o;
    };
    SOEXPORT std::string toString(const Frame2d &f);
    inline std::ostream &operator<<( std::ostream &s, const Frame2d &f )
    { return s << toString(f); }
    bool operator==( const Frame2d &f1, const Frame2d &f2 );

    // Describes the map's size and place in the world: everything except the actual cell values.
    struct MetaData {
        MetaData()
            : numCellsX(0),numCellsY(0),metresPerCell(0),offset(0,0,0) {}
        MetaData( int            pNumCellsX, 
                  int            pNumCellsY, 
                  double         pMetresPerCell,
                  const Frame2d &pOffset )
            : numCellsX(pNumCellsX),
              numCellsY(pNumCellsY),
              metresPerCell(pMetresPerCell),
              offset(pOffset) {}

        //! Are the world coordinates within the map?
        bool coordsWithinMap( const WorldCoords &worldCoords ) const
            {
                assert( offset.o == 0.0 );
                return ( worldCoords.x >= offset.p.x &&
                         worldCoords.y >= offset.p.y &&
                         worldCoords.x <  offset.p.x+numCellsX*metresPerCell &&
                         worldCoords.y <  offset.p.y+numCellsY*metresPerCell );
            }
        //! Are the world coordinates within the map?
        bool coordsWithinMap( double worldX, double worldY ) const
            { return coordsWithinMap(WorldCoords(worldX,worldY)); }

        //! The number of cells along the x axis
        int     numCellsX;
        //! The number of cells along the y axis
        int     numCellsY;
        //! The size of each cell
        double  metresPerCell;
        //! The global coordinates of the bottom-left corner of the bottom-left cell
        Frame2d offset;
    };
    SOEXPORT std::string toString(const MetaData &metaData );
    inline std::ostream &operator<<( std::ostream &s, const MetaData &m )
    { return s << toString(m); }
    bool operator==( const MetaData &m1, const MetaData &m2 );

/*!
     A (spatial) map of values: posit a 2D grid of cells over the world, 
     with a value stored at each cell.
    
  There are conceptually two coordinate systems:

  1) The global continuous coordinate system.
  2) The discrete grid-based coordinate system.

  In addition:

  3) The array of cells can be indexed by a single integer.

  The three methods of referring to cells are defined as follows:

@verbatim
     1) GLOBAL COORDINATE SYSTEM              2) CELL INDEXING                 3) DATA STORAGE
         worldCoords(double,double)            gridIndices(int,int)            e.g. OgMapData->data(index)
           _________________                   _________________                _________________
          |                 |     numCellsY-1 |  ^              |              |                 | 
 world    |                 |         ^       |  |              |              |                 | 
 y-axis   |                 |         .       |  |              |              |                 |
   ^      |                 |         . indY  |  |worldSizeY    |              |                 |
   |      |                 |         .       |  |              |              |                 |
   |      |                 |         .       |  |  worldSizeX  |              |----> index      |
   |  oX  |                 |         .       |<-|------------->|              |---------------->|
   |----->|_________________|         0       |__v______________|              |_________________|
   |      ^                                  
   |      | oY       world                             indX                   index: 0 -> sizeX * sizeY - 1
   |______|_____>    x-axis                    0 - - - - - - -> numCellsX-1

   (oX = offset.p.x, oY = offset.p.y)

   (note: in (1), the map is aligned with the world axes.  This need not be the case.)
@endverbatim
*/
    template<typename T>
    class SOEXPORT GenericMap
    {
    public:

        explicit GenericMap()
            : metaData_(0,0,0,Frame2d(0,0,0))
            { reallocate(0,0); }

        explicit GenericMap( int      numCellsX, 
                             int      numCellsY, 
                             double   offsetX,
                             double   offsetY,
                             double   offsetTheta,
                             double   metresPerCell,
                             const T &initialValue )
            : metaData_(numCellsX,numCellsY,metresPerCell,Frame2d(offsetX,offsetY,offsetTheta))
            {
                reallocate( metaData_.numCellsX, metaData_.numCellsY );
                fill( initialValue );
            }

        explicit GenericMap( int            numCellsX, 
                             int            numCellsY, 
                             const Frame2d &offset,
                             double         metresPerCell,
                             const T       &initialValue )
            : metaData_(numCellsX,numCellsY,metresPerCell,offset)
            {
                reallocate( metaData_.numCellsX, metaData_.numCellsY );
                fill( initialValue );
            }

        explicit GenericMap( int            numCellsX, 
                             int            numCellsY, 
                             const Frame2d &offset,
                             double         metresPerCell )
            : metaData_(numCellsX,numCellsY,metresPerCell,offset)
            { reallocate( metaData_.numCellsX, metaData_.numCellsY ); }

        explicit GenericMap( const MetaData &metaData )
            : metaData_(metaData)
            { reallocate( metaData_.numCellsX, metaData_.numCellsY ); }

        explicit GenericMap( const MetaData &metaData,
                             const T        &initialValue )
            : metaData_(metaData)
            {
                reallocate( metaData_.numCellsX, metaData_.numCellsY );
                fill( initialValue );
            }

        // member access functions
        
        //! Returns the number of cells along the x-axis 
        int numCellsX() const { return metaData_.numCellsX; };

        //! Returns the number of cells along the y-axis
        int numCellsY() const { return metaData_.numCellsY; };

        void setMetaData( const MetaData &metaData )
            {
                metaData_ = metaData;
                reallocate( metaData_.numCellsX, metaData_.numCellsY );
            }
        const MetaData &metaData() const { return metaData_; }

        //! Returns the size of the world, in metres, along the x-axis
        double worldSizeX() const { return numCellsX()*metresPerCell(); };
        //! Returns the size of the world, in metres, along the y-axis
        double worldSizeY() const { return numCellsY()*metresPerCell(); };

        //! Set the offset (global coordinates of the bottom-left corner of the bottom-left cell)
        Frame2d &offset() { return metaData_.offset; };
        //! Get the offset (global coordinates of the bottom-left corner of the bottom-left cell)
        const Frame2d &offset() const { return metaData_.offset; };
        
        //! Set the size of each cell
        void setMetresPerCell( double mPerCell ) { metaData_.metresPerCell = mPerCell; };
        //! get the size of each cell
        double metresPerCell() const { return metaData_.metresPerCell; };

        //! Set values
        T *data() { return &data_[0]; };
        //! Get values
        const T *data() const { return &data_[0]; };

        //! Get values as a vector
        const std::vector<T> &dataVec() const { return data_; };
        std::vector<T> &dataVec() { return data_; };

        //! Get the x world coord of a grid cell.
        //! (Returns the coords of the centre of the cell)
        //! gridX doesn't have to be within the map bounds.
        double worldXCoord( int gridX ) const
            { return double(offset().p.x + (gridX+0.5)*metresPerCell()); }
        //! Get the y world coord of a grid cell.
        //! (Returns the coords of the centre of the cell)
        //! gridY doesn't have to be within the map bounds.
        double worldYCoord( int gridY ) const
            { return double(offset().p.y + (gridY+0.5)*metresPerCell()); }

        //! Get the world coordinates of a grid cell.
        //! (Returns the coords of the centre of the cell)
        //! gridX and gridY don't have to be within the map bounds.
        void getWorldCoords( int gridX, int gridY, double &worldX, double &worldY ) const
            {
                WorldCoords world = worldCoords( GridIndices(gridX,gridY) );
                worldX = world.x; worldY = world.y;
            }
        //! Get the world coordinates of a grid cell.
        //! (Returns the coords of the centre of the cell)
        //! gridX and gridY don't have to be within the map bounds.
        WorldCoords worldCoords( const GridIndices &gridIndices ) const
            { 
                // Todo: handle non-zero orientation.
                assert(offset().o == 0.0 );

                return WorldCoords( worldXCoord(gridIndices.x), worldYCoord(gridIndices.y) );
            }
        WorldCoords worldCoords( int gridX, int gridY ) const
            { return worldCoords( GridIndices(gridX,gridY) ); }

        //! Get the grid cell indices of a point in the world
        //! worldX and worldY don't have to be within the map bounds.
        void getCellIndices( double worldX, double worldY, int &gridX, int &gridY ) const
            {
                gridX = worldToIndexX( worldX );
                gridY = worldToIndexY( worldY );
            }
        GridIndices gridIndices( const WorldCoords &worldCoords ) const
            { return GridIndices( worldToIndexX(worldCoords.x), worldToIndexY(worldCoords.y) ); }
        GridIndices gridIndices( double worldX, double worldY ) const
            { return gridIndices( WorldCoords(worldX,worldY) ); }

        // Look up the index into the flat data array.
        double dataIndex( const GridIndices &gridIndices ) const
            { return gridIndices.y*numCellsX() + gridIndices.x; }
        double dataIndex( int x, int y ) const
            { return dataIndex(GridIndices(x,y)); }

        //! Set a cell indexed by map coords
        //! (no bounds checking is performed)
        T &gridCell( int indX, int indY )
            { return gridCell(GridIndices(indX,indY)); }
        //! Get a cell, indexed by map coords
        //! (no bounds checking is performed)
        const T &gridCell( int indX, int indY ) const
            { return gridCell(GridIndices(indX,indY)); }

        //! Set a cell indexed by map coords
        //! (no bounds checking is performed)
        T &gridCell( const GridIndices &gridIndices )
        { return data_[dataIndex(gridIndices)]; }
        //! Get a cell, indexed by map coords
        //! (no bounds checking is performed)
        const T &gridCell( const GridIndices &gridIndices ) const
        { return data_[dataIndex(gridIndices)]; }
    
        

        //! Set a cell indexed by world coords 
        //! (no bounds checking is performed)
        T &worldCell( double worldX, double worldY )
            { return worldCell(WorldCoords(worldX,worldY)); }
        //! Get a cell, indexed by world coords
        //! (no bounds checking is performed)
        const T &worldCell( double worldX, double worldY ) const
            { return worldCell(WorldCoords(worldX,worldY)); }

        //! Set a cell indexed by world coords 
        //! (no bounds checking is performed)
        T &worldCell( const WorldCoords &worldCoords )
        { 
            assert( offset().o == 0.0 );
            return gridCell( gridIndices(worldCoords) );
        }
        //! Get a cell, indexed by world coords
        //! (no bounds checking is performed)
        const T &worldCell( const WorldCoords &worldCoords ) const
        { 
            assert( offset().o == 0.0 );
            return gridCell( gridIndices(worldCoords) );
        }

        //! Tries to get a cell (bounds check performed). Returns TRUE if successful, FALSE if the cell is outside the map.
        bool tryGridCell( int indX, int indY, T &cell ) const
            { return tryGridCell( GridIndices(indX,indY), cell ); }

        //! Tries to get a cell (bounds check performed). Returns TRUE if successful, FALSE if the cell is outside the map.
        bool tryWorldCell( double worldX, double worldY, T &cell ) const
            { return tryWorldCell( WorldCoords(worldX,worldY), cell ); }

        //! Tries to get a cell (bounds check performed). Returns TRUE if successful, FALSE if the cell is outside the map.
        bool tryGridCell( const GridIndices &indices, T &cell ) const
            { return tryWorldCell( worldCoords(indices), cell); }

        //! Tries to get a cell (bounds check performed). Returns TRUE if successful, FALSE if the cell is outside the map.
        bool tryWorldCell( const WorldCoords &worldCoords, T &cell ) const
            {
                if ( !coordsWithinMap( worldCoords ) ) return false;
                cell = worldCell( worldCoords );
                return true;
            }

        //! translate a world coord to a map coord
        int worldToIndexX( double worldX ) const
        { assert(offset().o==0.0); return (int) std::floor( (worldX-offset().p.x)/metresPerCell() ); }
        int worldToIndexY( double worldY ) const
        { assert(offset().o==0.0); return (int) std::floor( (worldY-offset().p.y)/metresPerCell() ); }
    
        //! Change the size of the map
        void reallocate( int numCellsX, int numCellsY )
            {
                metaData_.numCellsX = numCellsX;
                metaData_.numCellsY = numCellsY;
                data_.resize( metaData_.numCellsX * metaData_.numCellsY );
            }

        //! Fill the map with the given value
        void fill( const T &cellValue )
            {
                std::fill( data_.begin(), data_.end(), cellValue );
            }
                
    
        //! Are the world coordinates within the map?
        bool coordsWithinMap( double worldX, double worldY ) const
            { return coordsWithinMap(WorldCoords(worldX,worldY)); }
        //! Are the world coordinates within the map?
        bool coordsWithinMap( const WorldCoords &worldCoords ) const
        { return metaData_.coordsWithinMap(worldCoords); }

        //! Is the grid cell within the map?
        bool cellWithinMap( double gridX, double gridY ) const
            { return cellWithinMap(GridIndices(gridX,gridY)); }
        //! Is the grid cell within the map?
        bool cellWithinMap( const GridIndices &gridIndices ) const
        { 
            return ( gridIndices.x >= 0 &&
                     gridIndices.y >= 0 &&
                     gridIndices.x <  numCellsX() &&
                     gridIndices.y <  numCellsY() );
        }

    private:

        // Describes the map's size and position in the world
        MetaData metaData_;
        // The values
        std::vector<T> data_;
    };

    template<typename T>
    SOEXPORT std::ostream &operator<<( std::ostream &s, const GenericMap<T> &o )
    {
        s << " OgMap: \n"
          << o.metaData();
        return s;
    }
}

#endif
