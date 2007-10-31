/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "homeI.h"

using namespace orcaice;

HomeI::HomeI( ComponentInterfaceFlag flag, const orcaice::Context& context )
{
    IceUtil::Mutex::Lock lock(mutex_);

    // start the up-time clock
    startTime_ = IceUtil::Time::now();

    // special cases: add standard interfaces (depending on the startup flag )
    orca::ProvidedInterface provided;

    // first, add the Home interface itself, if we are here that means the HomeInterface flag is on
    provided.name = toHomeIdentity( context.name() );
    provided.id = "::orca::Home";
    homeData_.comp.provides.push_back( provided );

    if ( flag & TracerInterface ) {
        provided.name = "tracer";
        provided.id = "::orca::Tracer";
        homeData_.comp.provides.push_back( provided );
    }

    if ( flag & StatusInterface ) {
        provided.name = "status";
        provided.id = "::orca::Status";
        homeData_.comp.provides.push_back( provided );
    }

    orcaice::setToNow( homeData_.timeStamp );
    
    // save component properties
    properties_ = context.properties()->getPropertiesForPrefix("");
}

orca::HomeData
HomeI::getInterfaces(const ::Ice::Current& ) const
{
    //std::cout << "Sending data back" << std::endl;
    IceUtil::Mutex::Lock lock(mutex_);

    IceUtil::Time timeUp = IceUtil::Time::now() - startTime_;
    // we only return the number of seconds
    homeData_.timeUp = (int)timeUp.toSeconds();

    return homeData_;
}

int
HomeI::getTimeUp(const ::Ice::Current& ) const
{
    IceUtil::Mutex::Lock lock(mutex_);

    IceUtil::Time timeUp = IceUtil::Time::now() - startTime_;
    // we only return the number of seconds
    return (int)timeUp.toSeconds();
}

orca::ComponentProperties 
HomeI::getProperties(const ::Ice::Current& ) const
{
    //std::cout << "Sending data back" << std::endl;
    IceUtil::Mutex::Lock lock(mutex_);

    return properties_;
}

void
HomeI::addProvidedInterface( const orca::ProvidedInterface& iface ) 
{
    IceUtil::Mutex::Lock lock(mutex_);

    homeData_.comp.provides.push_back( iface );
    orcaice::setToNow( homeData_.timeStamp );
}

void 
HomeI::addRequiredInterface( const orca::RequiredInterface& iface )
{
    IceUtil::Mutex::Lock lock(mutex_);

    homeData_.comp.requires.push_back( iface );
    orcaice::setToNow( homeData_.timeStamp );
}

void 
HomeI::removeProvidedInterface( const std::string& name )
{
    IceUtil::Mutex::Lock lock(mutex_);
}

void 
HomeI::removeRequiredInterface( const std::string& name )
{
    IceUtil::Mutex::Lock lock(mutex_);
}
