/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_ICE_PTR_BUFFER_H
#define ORCAICE_ICE_PTR_BUFFER_H

#include <orcaice/buffer.h>

namespace orcaice
{

/*!
 *  @brief A thread-safe data pipe with buffer semantics for Ice smart pointers.
 * 
 *  @see PtrBuffer, PtrNotify
 */
template<class Type>
class PtrBuffer : public Buffer<Type>
{
    using Buffer<Type>::queue_;

public:
    /*!
     *  Buffer depth:
     *      - positive numbers to specify finite depth,
     *      - negative numbers for infinite depth, limited by memory size (DEFAULT)
     *      - zero is undefined
     *  Default buffer type is CIRCULAR, of depth 1.
     */
    PtrBuffer( int depth=1, BufferType type=BufferTypeCircular );
    
protected:
    virtual void internalGet( Type & obj ) const ;

    virtual void internalGet( Type & obj, unsigned int n ) const ;
    
    virtual void internalPush( const Type & obj );
};

template<class Type>
PtrBuffer<Type>::PtrBuffer( int depth, BufferType type )
    : Buffer<Type>(depth,type)
{
}

template<class Type>
void PtrBuffer<Type>::internalGet( Type & obj ) const
{
    // create a shallow copy (see Ice manual p.210)
    Ice::ObjectPtr superclass = queue_.front()->ice_clone();
    obj = Type::dynamicCast( superclass );
}

template<class Type>
void PtrBuffer<Type>::internalGet( Type & obj, unsigned int n ) const
{
    // create a shallow copy (see Ice manual p.210)
    Ice::ObjectPtr superclass = queue_[n]->ice_clone();
    obj = Type::dynamicCast( superclass );
}

template<class Type>
void PtrBuffer<Type>::internalPush( const Type & obj )
{
    Ice::ObjectPtr clone = obj->ice_clone();
    Type subclass = Type::dynamicCast( clone );
    queue_.push_back( subclass );
}

} // end namespace

#endif
