/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_OG_BUFFER_H
#define ORCA_OG_BUFFER_H

#include <map>

#include <orca/ogfusion.h>
//#include "ogfeature.h"

/*!
    A buffer for Occupancy (Certainty) Grid representation.
    
  @author Alex Makarenko
*/
namespace ogfusion{

class OgBuffer
{
public:

    // What to do when we try to insert an element that's already there
    enum ReplacementPolicy {
        PolicyReplaceLikelihood, // Most recent takes precedence
        PolicyAddLikelihood,     // Fuse the two
        PolicyMaxLikelihood      // Take the maximum likelihood of occupancy
    };

    ////////////////////////////////////////

    OgBuffer();
    ~OgBuffer();

    bool isEmpty() const;
    unsigned int size() const;
    void clear();

    orca::OgCellLikelihood& begin();
    orca::OgCellLikelihood& end();

    orca::OgCellLikelihood& insert( int ind, const orca::OgCellLikelihood &obs, ReplacementPolicy policy );

    void erase( const int ind ) {};
    void eraseFront();
    void eraseBack();

    void popFront( orca::OgCellLikelihood& obs);
    void popBack( orca::OgCellLikelihood& obs );

    void operator[] (const int ) const {};

    const std::map<int,orca::OgCellLikelihood> &bufferAsMap() const { return buffer_; }

private:


    std::map<int,orca::OgCellLikelihood> buffer_;

    //! A memory space for a single feature, stored here after popping from buffer_.
    orca::OgCellLikelihood currentFeature_;

};

// INLINE FUNCTIONS ==================

inline bool OgBuffer::isEmpty() const
{ return buffer_.empty(); };

inline unsigned int OgBuffer::size() const
{ return buffer_.size(); };

inline void OgBuffer::clear()
{ buffer_.clear(); };

inline orca::OgCellLikelihood& OgBuffer::begin()
{ return buffer_.begin()->second; };

inline orca::OgCellLikelihood& OgBuffer::end()
{ return buffer_.end()->second; };

}
#endif
