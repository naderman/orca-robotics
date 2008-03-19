/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_ICE_PTR_BUFFER_H
#define ORCAICE_ICE_PTR_BUFFER_H

#include <gbxsickacfr/gbxiceutilacfr/buffer.h>
#include <Ice/Ice.h>

namespace orcaice
{

/*!
 *  @brief A thread-safe data pipe with buffer semantics for Ice smart pointers.
 * 
 *  @note Works only with smart pointers to classes auto-generated from Slice 
 *  definitions.
 *
 *  @see PtrStore, PtrNotify
 */
template<class Type>
class PtrBuffer : public gbxsickacfr::gbxiceutilacfr::Buffer<Type>
{
    using gbxsickacfr::gbxiceutilacfr::Buffer<Type>::queue_;

public:
    /*!
     *  Buffer depth:
     *      - positive numbers to specify finite depth,
     *      - negative numbers for infinite depth, limited by memory size
     *      - zero is undefined
     */
    PtrBuffer( int depth, gbxsickacfr::gbxiceutilacfr::BufferType type );
    
protected:
    virtual void internalGet( Type & obj ) const ;

    virtual void internalGet( Type & obj, unsigned int n ) const ;
    
    virtual void internalPush( const Type & obj );
};

template<class Type>
PtrBuffer<Type>::PtrBuffer( int depth, gbxsickacfr::gbxiceutilacfr::BufferType type )
    : gbxsickacfr::gbxiceutilacfr::Buffer<Type>(depth,type)
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
