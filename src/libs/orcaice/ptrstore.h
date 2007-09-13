/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_ICE_PTR_STORE_H
#define ORCAICE_ICE_PTR_STORE_H

#include <orcaice/store.h>
#include <Ice/Ice.h>

namespace orcaice
{

/*!
 *  @brief Thread-safe storage for a single data object represented by an Ice smart pointer.
 * 
 *  @note Replaces the deprecated PtrProxy class.
 *  @see PtrBuffer, PtrNotify
 */
template<class Type>
class PtrStore : public Store<Type>
{
    using Store<Type>::obj_;
    
protected:
    virtual void internalGet( Type & obj ) const ;
    
    virtual void internalSet( const Type & obj );
};

template<class Type>
void PtrStore<Type>::internalGet( Type & obj ) const
{
    // create a shallow copy (see Ice manual p.210)
    Ice::ObjectPtr superclass = obj_->ice_clone();
    obj = Type::dynamicCast( superclass );
}

template<class Type>
void PtrStore<Type>::internalSet( const Type & obj )
{
    Ice::ObjectPtr clone = obj->ice_clone();
    Type subclass = Type::dynamicCast( clone );
    obj_ = subclass;
}

} // end namespace

#endif
