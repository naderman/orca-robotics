/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_TYPEMAP_H
#define ORCA2_TYPEMAP_H

#include <vector>
#include "cell2d.h"

namespace orcapathplan
{
    //! A 2-dimensional gridmap of a certain type, e.g. float, integer
    //! @author Tobias Kaupp
    template<typename Type>
    class TypeMap
    {

    public:
        TypeMap( unsigned int szx=0, unsigned int sz=0 );
        ~TypeMap() {};

        //! Returns the number of cells along the x-axis 
        unsigned int sizeX() const { return sizeX_; };

        //! Returns the number of cells along the y-axis
        unsigned int sizeY() const { return sizeY_; };

        //! Change the size of the map
        void resize( unsigned int szx, unsigned int szy );

        //! Fill the map with a value
        void fill( Type val );
    
        //! Check whether a cell is within the grid
        bool isInGrid( int x, int y ) const { return (x >= 0 && x < (int)sizeX_ && y >= 0 && y < (int)sizeY_ ); };

        //! Convenience function, see above
        bool isInGrid( Cell2D & cell ) const { return isInGrid( cell.x(), cell.y() ); };
      
        //! Gets the value of cell with index x,y. Will return NaN if cell is outside the map
        Type element( int x, int y ) const { return ( x >= (int)sizeX_ || y >= (int)sizeY_ || x < 0 || y < 0 ) ? NAN : data_[x][y]; };
        //! Gets the value of cell with indexes x,y, will return FALSE if outside map, otherwise TRUE
        bool tryElement( int x, int y, Type & val ) const;

        //! Gets the value of cell c. Will return NaN if cell is outside the map
        float element( Cell2D c ) const { return element( c.x(), c.y() ); };
        //! Gets the value of cell c, will return FALSE if outside map, otherwise TRUE
        bool tryElement( Cell2D c, Type & val ) const { return tryElement(c.x(),c.y(),val); };
        
        //! Sets the value of a cell, size limits are checked
        void setElement( int x, int y, Type val );

        //! Sets the value of a cell, size limits are checked
        void setElement( Cell2D c, Type val ) { setElement(c.x(),c.y(),val); };
        
            
    private:
        unsigned int sizeX_;
        unsigned int sizeY_;
        std::vector< std::vector< Type > > data_;
    };
    
    template<typename Type>
    TypeMap<Type>::TypeMap( unsigned int szx, unsigned int szy )
        : sizeX_( 0 ), sizeY_( 0 )
    {
        resize( szx, szy );
    }
    
    template<typename Type>
    void 
    TypeMap<Type>::resize( unsigned int szx, unsigned int szy )
    {
        data_.resize(szx);

        typedef typename std::vector< std::vector<Type> >::iterator dataIterator;
        for ( dataIterator it=data_.begin(); it!=data_.end(); it++ )
        {
            it->resize(szy);
        }
        
        sizeX_ = szx;
        sizeY_ = szy;
    }
    
    template<typename Type>
    void 
    TypeMap<Type>::fill(Type val)
    {
        typedef typename std::vector< std::vector<Type> >::iterator dataIterator;
        for( dataIterator it=data_.begin(); it!=data_.end(); it++ )
        {
            std::fill( it->begin(), it->end(), val );
        }
    }
    
    template<typename Type>
    bool 
    TypeMap<Type>::tryElement( int x, int y, Type & val ) const
    { 
        //will return -1.0 if outside the grid
        if (!isInGrid( x, y )) return false;
        val = element( x,y );
        return true;
    }
    
    
    template<typename Type>
    void 
    TypeMap<Type>::setElement( int x, int y, Type val )
    { 
        //will not set any value if outside the grid
        if( x >= (int)sizeX_ || y >= (int)sizeY_ || x < 0 || y < 0 ) return;
        data_[x][y] = val;
    }

}
#endif

