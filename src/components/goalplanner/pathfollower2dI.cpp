/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include "pathfollower2dI.h"

using namespace std;
using namespace orca;
using namespace orcaice;

namespace goalplanner {

PathFollower2dI::PathFollower2dI( orcaice::Proxy<orca::PathFollower2dData> &pathPipe,
                                  orcaice::Proxy<bool> &activationPipe,
                                  orca::PathFollower2dPrx localNavPrx )
    : pathPipe_(pathPipe),
      activationPipe_(activationPipe),
      localNavPrx_(localNavPrx)
{
}

orca::PathFollower2dData
PathFollower2dI::getData( const ::Ice::Current& ) const
{
    cout<<"TRACE(pathfollower2dI.cpp): getData()" << endl;
    orca::PathFollower2dData data;
    if ( pathPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "No path has been set" );
    }
    pathPipe_.get( data );
    return data;
}

void
PathFollower2dI::setData( const ::orca::PathFollower2dData& data, bool activateImmediately, const ::Ice::Current& )
{
    cout<<"TRACE(pathfollower2dI.cpp): Received new path: " << orcaice::toString(data) << endl;
    cout<<"TRACE(pathfollower2dI.cpp): activateImmediately: " << activateImmediately << endl;

    // Sanity check
    std::string insanityReason;
    if ( !orcaice::isSane( data, insanityReason ) )
    {
        throw orca::MalformedParametersException( insanityReason );
    }

    // give the data to the buffer
    pathPipe_.set( data );
    activationPipe_.set( activateImmediately );
}

void
PathFollower2dI::activateNow( const ::Ice::Current& )
{
//     cout << "TRACE(pathfollower2dI.cpp):activateNow: passing on to localnav." << endl;
    try {
        localNavPrx_->activateNow(); 
    } catch ( const Ice::Exception & e ) {
        cout << "Error: Exception when calling activateNow on localnav: " << e << endl;
    }
}

int
PathFollower2dI::getWaypointIndex( const ::Ice::Current& ) const
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
PathFollower2dI::getAbsoluteActivationTime(::orca::Time &activationTime, const Ice::Current&) const
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
PathFollower2dI::getRelativeActivationTime(double &secondsSinceActivation, const Ice::Current&) const
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
PathFollower2dI::enabled(const ::Ice::Current&) const
{
//     cout << "TRACE(pathfollower2dI.cpp):enabled: passing request on to localnav" << endl;
    try {
        return localNavPrx_->enabled();
    } catch ( const Ice::Exception & e ) {
        cout << "Error: Exception when calling activateNow on localnav: " << e << endl;
    }
    return false;
}

void
PathFollower2dI::subscribe( const ::orca::PathFollower2dConsumerPrx& subscriber, const ::Ice::Current& )
{
    cout<<"TRACE(pathfollower2dI.cpp): subscribe()"<<endl;
    return localNavPrx_->subscribe( subscriber );
}

void
PathFollower2dI::unsubscribe( const ::orca::PathFollower2dConsumerPrx& subscriber, const ::Ice::Current& )
{
    cout<<"TRACE(pathfollower2dI.cpp): unsubscribe()"<<endl;    
    return localNavPrx_->unsubscribe( subscriber );
}

}
