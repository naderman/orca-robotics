/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_ICE_PTR_NOTIFY_H
#define ORCAICE_ICE_PTR_NOTIFY_H

#include <gbxsickacfr/gbxiceutilacfr/notify.h>

#include <Ice/Object.h>

namespace orcaice
{

/*!
 *  @brief A data pipe with callback semantics for Ice smart pointers.
 * 
 *  @note Works only with smart pointers to classes auto-generated from Slice 
 *  definitions.
 *
 *  @see PtrBuffer, PtrStore
 */
template<class Type>
class PtrNotify : public gbxiceutilacfr::Notify<Type>
{
    using gbxiceutilacfr::Notify<Type>::handler_;
    
protected:
    virtual void internalSet( const Type & obj );
};

template<class Type>
void PtrNotify<Type>::internalSet( const Type & obj )
{
    Ice::ObjectPtr clone = obj->ice_clone();
    const Type subclass = Type::dynamicCast( clone );
    
    handler_->handleData( subclass );
}

} // end namespace

#endif
