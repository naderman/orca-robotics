/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
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

orca::OgCellLikelihood& 
OgBuffer::insert( const int ind, const orca::OgCellLikelihood &feature, ReplacementPolicy policy )
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
        if ( policy == PolicyReplaceLikelihood )
        {
            // replace existing feature
            it->second = feature;
        }
        else if ( policy == PolicyAddLikelihood )
        {
            // add new info to the existing feature (normalize or not?)
            it->second = ogfusion::add(it->second, feature );
        }
        else if ( policy == PolicyMaxLikelihood )
        {
            it->second.likelihood = max( it->second.likelihood, 
                                         feature.likelihood );
        }
        else
        {
            assert( false && "Unknown policy" );
        }
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
  
