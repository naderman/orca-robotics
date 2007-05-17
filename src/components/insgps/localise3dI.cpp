/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "localise3dI.h"

using namespace std;
using namespace orca;
using namespace insgps;

Localise3dI::Localise3dI( const VehicleDescription&  descr,
                          Driver*                   hwDriver,
                          const orcaice::Context & context )
    :   InsGpsI(context),
        descr_(descr),
        hwDriver_(hwDriver),
        context_(context)
{
    //
    // EXTERNAL PROVIDED INTERFACE: Localise3d
    //
    // Find IceStorm Topic to which we'll publish
    // the main topic is 'name/*@platform/component'
    topicPrx_ = orcaice::connectToTopicWithTag<Localise3dConsumerPrx>
            ( context_, localise3dPublisher_, "Localise3d" );
}

void
Localise3dI::publish()
{
   
    // blocking read with timeout (2000ms by default)
    // get localise3d
    read( localise3dData_ );
    
    // send the data to icestorm and to a buffer for direct connections
    localSetData( localise3dData_ );
            
    return;
}

//
// local calls
//

void
Localise3dI::read( ::orca::Localise3dData& data )
{
    hwDriver_->readLocalise3d( data );
}         


//
// remote calls
//

orca::Localise3dData
Localise3dI::getData(const Ice::Current& current) const
{
//     std::cout << "getData()" << std::endl;
    orca::Localise3dData data;
    // we don't need to pop the data here because we don't block on it.
    if ( localise3dDataBuffer_.isEmpty() )
    {
        cout << "ERROR(localise3dI.cpp): getData() was called when no data had been generated!!" << endl;
        throw orca::DataNotExistException( "Localise3d proxy is not populated yet" );
    }else{
        localise3dDataBuffer_.get( data );
    }
    return data;
}


::orca::Localise3dData
Localise3dI::getDataAtTime(const orca::Time& timeStamp, const ::Ice::Current& ) const
{
    std::string errString = "localise3dI:getDataAtTime(): NOT IMPLEMENTED YET";
    context_.tracer()->error( errString );
    throw orcaice::Exception( ERROR_INFO, errString );

    ::orca::Localise3dData data;
    return data;
}

::orca::VehicleDescription
Localise3dI::getDescription(const ::Ice::Current& ) const
{
    std::cout << "getDescription()" << std::endl;
    return descr_;
}

// Subscribe people
void 
Localise3dI::subscribe(const ::orca::Localise3dConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "subscribe()" << endl;
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer()->debug( ss.str(), 2 );    
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

// Unsubscribe people
void 
Localise3dI::unsubscribe(const ::orca::Localise3dConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "unsubscribe() Localise3d" << endl;
    topicPrx_->unsubscribe( subscriber );
}

// Set pva IMU Data
void
Localise3dI::localSetData( const ::orca::Localise3dData& data )
{
    // Stick it in the buffer so pullers can get it
    localise3dDataBuffer_.push( data );

    try {
        // push it to IceStorm
        localise3dPublisher_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down,
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}

