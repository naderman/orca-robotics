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
                                  orcaice::Proxy<orca::Localise2dData> &localiseDataBuffer,
                                  orca::PathFollower2dPrx localNavPrx,
                                  const IceStorm::TopicPrx & topicPrx )
    : pathPipe_(pathPipe),
      activationPipe_(activationPipe),
      localiseDataBuffer_(localiseDataBuffer),
      localNavPrx_(localNavPrx),
      topicPrx_(topicPrx)
{
    assert ( topicPrx_ != 0 );
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
    
    // Check whether we are localized
    Localise2dData localiseData;
    int ret = localiseDataBuffer_.getNext( localiseData, 1000 );

    // we are localized
    if (ret==0)
    {
        // do we have multiple hypotheses?
        if ( localiseData.hypotheses.size() != 1 )
        {
            throw orca::BusyException( "Multiple localization hypotheses. Will wait for a single hypothesis to execute your path");
        }
    }
}

void
PathFollower2dI::activateNow( const ::Ice::Current& )
{
    cout << "TRACE(pathfollower2dI.cpp):activateNow: passing on to localnav." << endl;
    localNavPrx_->activateNow(); 
}

int
PathFollower2dI::getWaypointIndex( const ::Ice::Current& ) const
{
    cout << "TRACE(pathfollower2dI.cpp): getWaypointIndex: passing request on to localnav" << endl;
    return localNavPrx_->getWaypointIndex(); 
}

::orca::Time 
PathFollower2dI::getAbsoluteStartTime(const Ice::Current&) const
{
    cout << "TRACE(pathfollower2dI.cpp): getAbsoluteStartTime: implement me" << endl;
    orca::Time t;
    setSane(t);
    return t;
}
    
double 
PathFollower2dI::getRelativeStartTime(const Ice::Current&) const
{
    cout << "TRACE(pathfollower2dI.cpp): getRelativeStartTime: implement me" << endl;
    return 0.0;
}

void 
PathFollower2dI::setEnabled( bool enabled, const ::Ice::Current& )
{
    cout << "TRACE(pathfollower2dI.cpp):setEnabled: passing request on to localnav" << endl;
    localNavPrx_->setEnabled(enabled); 
}

bool 
PathFollower2dI::enabled(const ::Ice::Current&) const
{
    cout << "TRACE(pathfollower2dI.cpp):enabled: passing request on to localnav" << endl;
    return localNavPrx_->enabled();
}

void
PathFollower2dI::subscribe( const ::orca::PathFollower2dConsumerPrx& subscriber, const ::Ice::Current& )
{
    cout<<"TRACE(pathfollower2dI.cpp): subscribe()"<<endl;
    assert ( topicPrx_ != 0 );
//     cout<<"TRACE(pathfollower2dI.cpp): topicPrx_: " << topicPrx_->ice_toString() << endl;
    IceStorm::QoS qos;
    topicPrx_->subscribe( qos, subscriber );
}

void
PathFollower2dI::unsubscribe( const ::orca::PathFollower2dConsumerPrx& subscriber, const ::Ice::Current& )
{
    cout<<"TRACE(pathfollower2dI.cpp): unsubscribe()"<<endl;    
    assert ( topicPrx_ != 0 );
    topicPrx_->unsubscribe( subscriber );
}

}
