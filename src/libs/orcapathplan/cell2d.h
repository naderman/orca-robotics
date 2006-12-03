/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_CELL2D_H
#define ORCA2_CELL2D_H

#include <iostream>
#include <vector>
#include <list>

namespace orcapathplan
{

    /*!
    @brief A light weight 2d cell reference containing a x and y index
    */
    class Cell2D
    {
    public:
        Cell2D( int x=0, int y=0);
        ~Cell2D() {};

        //! get x index of cell
        int x() const;
        //! get y index of cell
        int y() const;

        //! set x index of cell
        void setX( const int x);
        //! set y index of cell
        void setY( const int y);
        //! set both indeces of cell
        void set( const int x, const int y);

        //! arithmetic operator
        Cell2D & operator= ( const Cell2D & p );// { x_=p.x_; y_=p.y_; return *this; };
        //! arithmetic operator
        Cell2D & operator+= ( const Cell2D & );
        //! arithmetic operator
        Cell2D & operator-= ( const Cell2D & );
        //! arithmetic operator
        Cell2D & operator*= ( int );
        //! arithmetic operator, if division by zero, returns same cell
        Cell2D & operator/= ( int );

        //! arithmetic operator
        friend inline bool    operator== ( const Cell2D &, const Cell2D & );
        //! arithmetic operator
        friend inline bool    operator!= ( const Cell2D &, const Cell2D & );
        //! arithmetic operator
        friend inline const Cell2D operator+ ( const Cell2D &, const Cell2D & );
        //! arithmetic operator
        friend inline const Cell2D operator- ( const Cell2D &, const Cell2D & );
        //! arithmetic operator
        friend inline const Cell2D operator* ( const Cell2D &, int );
        //! arithmetic operator
        friend inline const Cell2D operator* ( int, const Cell2D & );
        //! arithmetic operator
        friend inline const Cell2D operator- ( const Cell2D & );
    private:
        int x_;
        int y_;
    };

    typedef std::vector<orcapathplan::Cell2D> Cell2DVector;
    typedef std::list<orcapathplan::Cell2D> Cell2DList;

    /*****************************************************************************
     * Cell2D utility functions
     *****************************************************************************/

    //! Find the geometric distance between two cells
    //! @relatesalso Cell2D
    double euclideanDistance( const Cell2D c1, const Cell2D c2 );

    //! Find the manhattan distance between two cells
    //! @relatesalso Cell2D
    int manhattanDistance( const Cell2D c1, const Cell2D c2 );

    //! Determine if a cell is within a square of size 2N+1 of another cell
    //! @relatesalso Cell2D
    bool isAdjacentN( const Cell2D c1, const Cell2D c2, unsigned int N );

    //! Check to see if a cell refers to (0,0)
    //! @relatesalso Cell2D
    bool isNull( const Cell2D c );

    /*
    index for adjacentCell and diagonalCell

      0    0    1

     3    [c]    1

      3    2    2

    index for surroundCell

      7    0    4

     3    [c]    1

      6    2    5
    */

    //! Returns a cell that is adjacent to the supplied cell.
    //! index goes from 0 -> 3, if outside this range, 0 is assumed.
    //! if the cell is on the edge behaviour is undefined.
    //! @relatesalso Cell2D
    Cell2D adjacentCell( const Cell2D & cell, const int i );

    //! Returns a cell that is diagonaly adjacent to the supplied cell.
    //! index goes from 0 -> 3, if outside this range, 0 is assumed.
    //! if the cell is on the edge behaviour is undefined.
    //! @relatesalso Cell2D
    Cell2D diagonalCell( const Cell2D & cell, const int i );

    //! Returns a cell that is directly adjacent or diagonally adjacent to the supplied cell.
    //! index goes from 0 -> 7, if outside this range, 0 is assumed.
    //! if the cell is on the edge behaviour is undefined.
    //! @relatesalso Cell2D
    Cell2D surroundCell( const Cell2D & cell, const int i );
    
    //! Printing this object
    //! @relatesalso Cell2D
    std::ostream& operator<<( std::ostream &s, const  Cell2D &obj );

    /*****************************************************************************
     * Cell2D inline member functions
     *****************************************************************************/

    inline Cell2D::Cell2D( int x, int y )
    { x_ = x; y_ = y; }

    inline int Cell2D::Cell2D::x() const
    { return x_; }

    inline int Cell2D::Cell2D::y() const
    { return y_; }

    inline void Cell2D::setX( const int x )
    { x_ = x; }

    inline void Cell2D::setY( const int y )
    { y_ = y; }

    inline void Cell2D::set( const int x, const int y )
    { x_ = x; y_ = y; }

    /*****************************************************************************
     * Cell2D arithmetic operator
     *****************************************************************************/

    //members
    inline Cell2D & Cell2D::operator= ( const Cell2D & p )
    { x_=p.x_; y_=p.y_; return *this; }
    
    inline Cell2D & Cell2D::operator+= ( const Cell2D &p )
    { x_+=p.x_; y_+=p.y_; return *this; }

    inline Cell2D & Cell2D::operator-= ( const Cell2D &p )
    { x_-=p.x_; y_-=p.y_; return *this; }

    inline Cell2D & Cell2D::operator*= ( int c )
    { x_*=c; y_*=c; return *this; }

    inline Cell2D & Cell2D::operator/= (int c)
    { if(c==0) return *this; x_/=c; y_/=c; return *this; }

    //friends
    inline bool operator==( const Cell2D &p1, const Cell2D &p2 )
    { return p1.x_ == p2.x_ && p1.y_ == p2.y_; }

    inline bool operator!=( const Cell2D &p1, const Cell2D &p2 )
    { return p1.x_ != p2.x_ || p1.y_ != p2.y_; }

    inline const Cell2D operator+( const Cell2D &p1, const Cell2D &p2 )
    { return Cell2D(p1.x_+p2.x_, p1.y_+p2.y_); }

    inline const Cell2D operator-( const Cell2D &p1, const Cell2D &p2 )
    { return Cell2D(p1.x_-p2.x_, p1.y_-p2.y_); }

    inline const Cell2D operator*( const Cell2D &p, int c )
    { return Cell2D(p.x_*c, p.y_*c); }

    inline const Cell2D operator*( int c, const Cell2D &p )
    { return Cell2D(p.x_*c, p.y_*c); }

    inline const Cell2D operator-( const Cell2D &p )
    { return Cell2D(-p.x_, -p.y_); }
}

#endif



