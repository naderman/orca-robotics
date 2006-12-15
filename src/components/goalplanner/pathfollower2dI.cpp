/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

PathFollower2dI::PathFollower2dI( orcaice::PtrProxy<orca::PathFollower2dDataPtr> &pathPipe,
                                  orcaice::PtrProxy<orca::Localise2dDataPtr> &localiseDataBuffer,
                                  const IceStorm::TopicPrx & topicPrx )
    : pathPipe_(pathPipe),
      localiseDataBuffer_(localiseDataBuffer),
      topicPrx_(topicPrx)
{
    assert ( topicPrx_ != 0 );
}

orca::PathFollower2dDataPtr
PathFollower2dI::getData( const ::Ice::Current& ) const
{
    cout<<"TRACE(pathfollower2dI.cpp): getData()" << endl;
    orca::PathFollower2dDataPtr data;
    if ( pathPipe_.isEmpty() )
    {
        data = new orca::PathFollower2dData;
        return data;
    }
    pathPipe_.get( data );
    return data;
}

void
PathFollower2dI::setData( const ::orca::PathFollower2dDataPtr &data, bool activateImmediately, const ::Ice::Current& )
{
    cout<<"TRACE(pathfollower2dI.cpp): Received new path: " << data << endl;
    cout<<"TRACE(pathfollower2dI.cpp): activateImmediately: " << activateImmediately << " .This flag will have no effect here!" << endl;

    // Sanity check
    std::string insanityReason;
    if ( !orcaice::isSane( data, insanityReason ) )
    {
        throw orca::MalformedParametersException( insanityReason );
    }

    // give the data to the buffer
    pathPipe_.set( data );
    
    // Check whether we are localized
    Localise2dDataPtr localiseData = new Localise2dData;
    int ret = localiseDataBuffer_.getNext( localiseData, 1000 );
    cout << "localiseDataBuffer returns " << ret << endl;
    if (ret==0)
    {
        if ( localiseData->hypotheses.size() != 1 )
        {
            throw orca::BusyException( "Currently more than one localization hypothesis. Will wait for a single hypothesis to execute your path");
        }
    }
}

void
PathFollower2dI::activateNow( const ::Ice::Current& )
{
    cout << "TRACE(pathfollower2dI.cpp): Received activateNow signal. Will have no effect here." << endl;
}

int
PathFollower2dI::getWaypointIndex( const ::Ice::Current& ) const
{
    cout<<"TRACE(pathfollower2dI.cpp): TODO: implement me." << endl;
    return -1;
}

void 
PathFollower2dI::setEnabled( bool enabled, const ::Ice::Current& )
{
    cout<<"TRACE(pathfollower2dI.cpp): TODO: implement me." << endl;
}
bool 
PathFollower2dI::enabled(const ::Ice::Current&) const
{
    cout<<"TRACE(pathfollower2dI.cpp): TODO: implement me." << endl;
    return true;
}

void
PathFollower2dI::subscribe( const ::orca::PathFollower2dConsumerPrx& subscriber, const ::Ice::Current& )
{
    cout<<"subscribe()"<<endl;
    assert ( topicPrx_ != 0 );
    cout<<"TRACE(pathfollower2dI.cpp): topicPrx_: " << topicPrx_->ice_toString() << endl;
    IceStorm::QoS qos;
    topicPrx_->subscribe( qos, subscriber );
}

void
PathFollower2dI::unsubscribe( const ::orca::PathFollower2dConsumerPrx& subscriber, const ::Ice::Current& )
{
    assert ( topicPrx_ != 0 );
    cout<<"unsubscribe()"<<endl;
    topicPrx_->unsubscribe( subscriber );
}

}
