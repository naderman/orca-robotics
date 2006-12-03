/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "ogbuffer.h"
#include "ogcellops.h"

using namespace std;

namespace ogfusion{


OgBuffer::OgBuffer()
{
}

OgBuffer::~OgBuffer()
{	
}

/*!
    No normalization?
*/
orca::OgCellLikelihood& OgBuffer::insertAdd( const int ind, orca::OgCellLikelihood& feature )
{
    // check if the element is already in the list
    map<int,orca::OgCellLikelihood>::iterator it = buffer_.find( ind );
    
    if ( it == buffer_.end() ) 	// element with specified key does not exist
    {
        //cout<<"OgBuffer::insertAdd: inserting feature "<<feature->index()<<endl;
        //buffer_.insert( make_pair( feature->index(),*feature ) );
        
        pair<map<int,orca::OgCellLikelihood>::iterator,bool> that;
        that = buffer_.insert( make_pair( ind, feature ) );
        it = that.first;
    }
    else  // element with specified key already exists
    {
	// add new info to the existing feature (normalize or not?)
        //TODO FUSION
	it->second=ogfusion::add(it->second, feature );
    }
    
    return it->second;
}

orca::OgCellLikelihood& OgBuffer::insertReplace(const int ind, orca::OgCellLikelihood& feature )
{
    // check if the element is already in the list
    map<int,orca::OgCellLikelihood>::iterator it = buffer_.find( ind );
    //pair<map<int,OgFeature>::iterator,bool> that;
    
    if ( it == buffer_.end() ) 	// element with specified key does not exist
    {
        //buffer_.insert( make_pair( feature->index(),*feature ) );
        
        pair<map<int,orca::OgCellLikelihood>::iterator,bool> that;
        that = buffer_.insert( make_pair( ind, feature ) );
        it = that.first;
    }
    else  // element with specified key already exists
    {
        // replace existing feature
        it->second = feature;
    }
    
    return it->second;
}

void OgBuffer::popFront( orca::OgCellLikelihood& f )
{
    // copy
    currentFeature_ = buffer_.begin()->second;
    
    buffer_.erase( buffer_.begin() );
    
    f = currentFeature_;
}

void OgBuffer::popBack( orca::OgCellLikelihood& f )
{
    std::map<int,orca::OgCellLikelihood>::iterator it = buffer_.end();
    it--;
    
    f = it->second;

    buffer_.erase( it );
}

void OgBuffer::eraseFront()
{
    buffer_.erase( buffer_.begin() );
}

void OgBuffer::eraseBack()
{
    std::map<int,orca::OgCellLikelihood>::iterator it = buffer_.end();
    it--;
    
    buffer_.erase( it );
}


}
  
