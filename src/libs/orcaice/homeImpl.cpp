/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include "homeImpl.h"

namespace orcaice
{

//////////////////////////////////////////////////////////////

class HomeI : public orca::Home
{
public:

    HomeI( HomeImpl &impl )
        : impl_(impl) {}
    virtual ~HomeI() {}

    // remote interface
    virtual orca::HomeData getInterfaces(const ::Ice::Current& )
        { return impl_.internalGetInterfaces(); }
    virtual int getTimeUp(const ::Ice::Current& )
        { return impl_.internalGetTimeUp(); }
private:
    HomeImpl &impl_;
};

//////////////////////////////////////////////////////////////

// HomeImpl::HomeImpl( ComponentInterfaceFlag flag, const orcaice::Context& context ) :
HomeImpl::HomeImpl( const orcaice::Context& context ) :
    interfaceName_( toHomeIdentity(context.name()) ),
    context_(context)
{
    // fill the store
    orca::HomeData data;
    orcaice::setToNow( data.timeStamp );
    data.timeUp = 0;
    dataStore_.set( data );
}

HomeImpl::~HomeImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
HomeImpl::initInterface( const orcaice::Context& context )
{
    // need recopy context, because at construction time not all services were registered.
    context_ = context;

    // Register with the adapter just like any other interface with the special identity
    // specified in toHomeIdentity(fqcn).
    // Later we will also register it as a well-known object.
    ptr_ = new HomeI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

orca::HomeData
HomeImpl::internalGetInterfaces()
{
    orca::HomeData data;
    dataStore_.get( data );

    orcaice::setToNow( data.timeStamp );
    data.timeUp = (Ice::Int)upTimer_.elapsedSec();

    return data;
}

int
HomeImpl::internalGetTimeUp()
{
    // we only return the number of seconds
    return (Ice::Int)upTimer_.elapsedSec();
}

void
HomeImpl::addProvidedInterface( const orca::ProvidedInterface& iface ) 
{
    orca::HomeData data;
    dataStore_.get( data );

    data.comp.provides.push_back( iface );

    dataStore_.set( data );
}

void 
HomeImpl::addRequiredInterface( const orca::RequiredInterface& iface )
{
    orca::HomeData data;
    dataStore_.get( data );

    data.comp.requires.push_back( iface );

    dataStore_.set( data );
}

void 
HomeImpl::removeProvidedInterface( const std::string& name )
{
    assert( !"removeProvidedInterface() is not implemented" );
}

void 
HomeImpl::removeRequiredInterface( const std::string& name )
{
    assert( !"removeRequiredInterface() is not implemented" );
}

}
