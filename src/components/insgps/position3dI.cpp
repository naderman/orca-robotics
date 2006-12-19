/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "position3dI.h"

using namespace std;
using namespace orca;
using namespace insgps;

Position3dI::Position3dI( Position3dDescriptionPtr  descr,
                          Driver*                   hwDriver,
                          const orcaice::Context & context )
    :   InsGpsI(context),
        descr_(descr),
        hwDriver_(hwDriver),
        position3dData_(new Position3dData),
        context_(context)
{
    //
    // EXTERNAL PROVIDED INTERFACE: Position3d
    //
    // Find IceStorm Topic to which we'll publish
    // the main topic is 'name/*@platform/component'
    topicPrx_ = orcaice::connectToTopicWithTag<Position3dConsumerPrx>
            ( context_, position3dPublisher_, "Position3d" );
}

void
Position3dI::publish()
{
   
    // blocking read with timeout (2000ms by default)
    // get position3d
    read( position3dData_ );
    
    // send the data to icestorm and to a buffer for direct connections
    localSetData( position3dData_ );
            
    return;
}

//
// local calls
//

void
Position3dI::read( ::orca::Position3dDataPtr& data )
{
    hwDriver_->readPosition3d( data );
}         


//
// remote calls
//

orca::Position3dDataPtr
Position3dI::getData(const Ice::Current& current) const
{
    std::cout << "getData()" << std::endl;

    // create a null pointer. data will be cloned into it.
    orca::Position3dDataPtr data;
    // we don't need to pop the data here because we don't block on it.
    if ( position3dDataBuffer_.isEmpty() )
    {
        cout << "ERROR(position3dI.cpp): getData() was called when no data had been generated!!" << endl;
        throw orca::DataNotExistException( "Position3d proxy is not populated yet" );
    }else{
        position3dDataBuffer_.get( data );
    }

    return data;
}


::orca::Position3dDescriptionPtr
Position3dI::getDescription(const ::Ice::Current& ) const
{
    std::cout << "getDescription()" << std::endl;
    return descr_;
}

// Subscribe people
void 
Position3dI::subscribe(const ::orca::Position3dConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "subscribe()" << endl;
    IceStorm::QoS qos;
    topicPrx_->subscribe( qos, subscriber );
}

// Unsubscribe people
void 
Position3dI::unsubscribe(const ::orca::Position3dConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "unsubscribe() Position3d" << endl;
    topicPrx_->unsubscribe( subscriber );
}

// Set pva IMU Data
void
Position3dI::localSetData( ::orca::Position3dDataPtr data )
{
    // Stick it in the buffer so pullers can get it
    position3dDataBuffer_.push( data );

    try {
        // push it to IceStorm
        position3dPublisher_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down,
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}

