/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "interfacemanager.h"

using namespace orcaifaceimpl;

InterfaceManager::InterfaceManager( const Ice::ObjectAdapterPtr& adapter ) :
    adapter_(adapter)
{
}

InterfaceManager::InterfaceManager( const Ice::ObjectAdapterPtr& adapter, const Ice::ObjectPtr& ptr, const Ice::ObjectPrx& prx ) :
    adapter_(adapter),
    ptr_(ptr),
    prx_(prx)
{
}


InterfaceManager::~InterfaceManager()
{
    IceUtil::Mutex::Lock lock( mutex_ );
}

Ice::ObjectPtr
InterfaceManager::ptr()
{
    IceUtil::Mutex::Lock lock( mutex_ );
    return ptr_;
}

Ice::ObjectPrx 
InterfaceManager::prx()
{
    IceUtil::Mutex::Lock lock( mutex_ );
    return prx_;
}

void 
InterfaceManager::setInterface( const Ice::ObjectPtr& ptr, const Ice::ObjectPrx& prx )
{
    IceUtil::Mutex::Lock lock( mutex_ );
    ptr_ = ptr;
    prx_ = prx;
}
