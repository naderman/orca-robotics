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
#include <iostream>

using namespace std;

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
    virtual orca::HomeData getData(const ::Ice::Current& )
        { return impl_.internalGetData(); }
private:
    HomeImpl &impl_;
};

//////////////////////////////////////////////////////////////

HomeImpl::HomeImpl( const orcaice::Context& context ) :
    interfaceName_( "home" ),
    context_(context)
{
    // fill the store
    orca::HomeData data;
    data.comp.name = context_.name();
    dataStore_.set( data );
}

HomeImpl::~HomeImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
HomeImpl::initInterface()
{
    ptr_ = new HomeI( *this );

    // previous method: adding Home to the component adapter
//     orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );

    // EXPERIMENTAL! adding as a facet to the Admin interface.
    try
    {
        context_.communicator()->addAdminFacet( ptr_, "Home" );
    }
    catch ( const std::exception& e )
    {
        stringstream ss;
        ss << "(while installng Home object) : "<<e.what();
        context_.tracer().error( ss.str() );
        context_.shutdown();
    }

    // manually to home registry
    orca::ProvidedInterface iface;
    iface.name = interfaceName_;
    iface.id   = "::orca::Home";
    context_.home().addProvidedInterface( iface );
}

orca::HomeData
HomeImpl::internalGetData()
{
    orca::HomeData data;
    dataStore_.get( data );

    orcaice::setToNow( data.timeStamp );

    return data;
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
