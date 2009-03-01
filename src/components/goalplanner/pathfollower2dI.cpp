/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include "pathfollower2dI.h"

using namespace std;
using namespace orca;
using namespace orcaice;

namespace goalplanner {

PathFollower2dI::PathFollower2dI( gbxiceutilacfr::Store<orca::PathFollower2dData> &pathPipe,
                                  gbxiceutilacfr::Store<bool> &activationPipe,
                                  orca::PathFollower2dPrx localNavPrx,
                                  const orcaice::Context &context )
    : pathPipe_(pathPipe),
      activationPipe_(activationPipe),
      localNavPrx_(localNavPrx),
      context_(context)
{
}

orca::PathFollower2dData
PathFollower2dI::getData( const ::Ice::Current&)
{
    return localNavPrx_->getData();
}

void
PathFollower2dI::setData( const ::orca::PathFollower2dData& data, bool activateImmediately, const ::Ice::Current& )
{
    cout<<"TRACE(pathfollower2dI.cpp): Received new path: " << orcaobj::toString(data) << endl;
    cout<<"TRACE(pathfollower2dI.cpp): activateImmediately: " << activateImmediately << endl;

    // Sanity check
    std::string insanityReason;
    if ( !orcaobj::isSane( data, insanityReason ) )
    {
        stringstream ss;
        ss << "Path not valid.  Path was: " << orcaobj::toVerboseString(data) << endl;
        ss << "Problem: " << insanityReason;
        context_.tracer().warning( ss.str() );
        throw orca::MalformedParametersException( ss.str() );
    }

    // give the data to the buffer
    pathPipe_.set( data );
    activationPipe_.set( activateImmediately );
}

void
PathFollower2dI::activateNow( const ::Ice::Current& )
{
    cout << "TRACE(pathfollower2dI.cpp):activateNow: passing on to localnav." << endl;
    try {
        activationPipe_.set( true );
        localNavPrx_->activateNow(); 
    } catch ( const Ice::Exception & e ) {
        cout << "Error: Exception when calling activateNow on localnav: " << e << endl;
    }
}

int
PathFollower2dI::getWaypointIndex( const ::Ice::Current&)
{
//     cout << "TRACE(pathfollower2dI.cpp): getWaypointIndex: passing request on to localnav" << endl;
    try {
        return localNavPrx_->getWaypointIndex(); 
    } catch ( const Ice::Exception & e ) {
        cout << "Error: Exception when calling activateNow on localnav: " << e << endl;
    }
    return -1;
}

bool
PathFollower2dI::getAbsoluteActivationTime(::orca::Time &activationTime, const Ice::Current&)
{
//     cout << "TRACE(pathfollower2dI.cpp): getAbsoluteActivationTime: passing request on to localnav" << endl;
    try {
        bool ret = localNavPrx_->getAbsoluteActivationTime( activationTime );
        return ret;
    } catch ( const Ice::Exception & e ) {
        cout << "Error: Exception when calling activateNow on localnav: " << e << endl;
    }
    return false;
}
    
bool 
PathFollower2dI::getRelativeActivationTime(double &secondsSinceActivation, const Ice::Current&)
{
//     cout << "TRACE(pathfollower2dI.cpp): getRelativeActivationTime: passing request on to localnav" << endl;
    try {
        bool ret =  localNavPrx_->getRelativeActivationTime( secondsSinceActivation );
        return ret;
    } catch ( const Ice::Exception & e ) {
        cout << "Error: Exception when calling activateNow on localnav: " << e << endl;
    }
    return false;
}

void 
PathFollower2dI::setEnabled( bool enabled, const ::Ice::Current& )
{
//     cout << "TRACE(pathfollower2dI.cpp):setEnabled: passing request on to localnav" << endl;
    try {
        localNavPrx_->setEnabled(enabled); 
    } catch ( const Ice::Exception & e ) {
        cout << "Error: Exception when calling activateNow on localnav: " << e << endl;
    }
}

bool 
PathFollower2dI::enabled(const ::Ice::Current&)
{
//     cout << "TRACE(pathfollower2dI.cpp):enabled: passing request on to localnav" << endl;
    try {
        return localNavPrx_->enabled();
    } catch ( const Ice::Exception & e ) {
        cout << "Error: Exception when calling activateNow on localnav: " << e << endl;
    }
    return false;
}

IceStorm::TopicPrx
PathFollower2dI::subscribe( const ::orca::PathFollower2dConsumerPrx& subscriber, const ::Ice::Current& )
{
    cout<<"TRACE(pathfollower2dI.cpp): subscribe()"<<endl;
    return localNavPrx_->subscribe( subscriber );
}

}
