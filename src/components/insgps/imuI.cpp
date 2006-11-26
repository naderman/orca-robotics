/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Matthew Ridley, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "imuI.h"

using namespace std;
using namespace orca;
using namespace insgps;

ImuI::ImuI( ImuDescriptionPtr   descr,
            Driver*      hwDriver,
            orcaice::Context   context )
    : InsGpsI(context),
      descr_(descr),
      hwDriver_(hwDriver),
      imuData_(new ImuData),
      context_(context)
{
    //
    // EXTERNAL PROVIDED INTERFACE: Imu
    //
    // Find IceStorm Topic to which we'll publish
    // the main topic is 'name/*@platform/component'
    topicPrx_ = orcaice::connectToTopicWithTag<ImuConsumerPrx>
        ( context_, imuPublisher_, "Imu" );
}

void
ImuI::publish()
{
//     const int TIME_BETWEEN_HEARTBEATS  = 10000;  // ms
//     IceUtil::Time lastHeartbeatTime = IceUtil::Time::now();
// 
//     try 
//     {
//         context_.tracer()->debug( "TRACE(gpsI::run()): ImuI thread is running", 5);
//         
//         ImuDataPtr imuData = new ImuData;
//         
//         //
//         // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks
//         // for requests to shut down. So we have to avoid getting stuck in a loop anywhere within this main loop.
//         //
//         while ( isActive() )
//         {
//             if ( hwDriver_->isEnabled() )
//             {
   
                // blocking read with timeout (2000ms by default)
                // get the imu position
                read( imuData_ );
                
                // send the data to icestorm and to a buffer for direct connections
                localSetData( imuData_ );
//             }
//             else
//             {
//                 // Wait for someone to enable us
//                 IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));
//             }
// 
//             if ( (IceUtil::Time::now()-lastHeartbeatTime).toMilliSecondsDouble() >= TIME_BETWEEN_HEARTBEATS )
//             {
//                 if ( hwDriver_->isEnabled() )
//                 {
//                     context_.tracer()->heartbeat("InsGps enabled. " + hwDriver_->heartbeatMessage() );
//                 }
//                 else
//                 {
//                     context_.tracer()->heartbeat( "InsGps disabled." );
//                 }
//                 lastHeartbeatTime = IceUtil::Time::now();
//             }
//         } // end of while
//     } // end of try
//     catch ( Ice::CommunicatorDestroyedException &e )
//     {
//         // This is OK: it means that the communicator shut down (eg via Ctrl-C)
//         // somewhere in mainLoop.
//     }
// 
//     // wait for the component to realize that we are quitting and tell us to stop.
//     waitForStop();
//     
//     // InsGps hardware will be shut down in the driver's destructor.
//     context_.tracer()->debug( "dropping out from run()", 5 );
            
}

//
// local calls
//

void
ImuI::read( ::orca::ImuDataPtr& data )
{
    hwDriver_->readImu( data );
}         


//
// remote calls
//

orca::ImuDataPtr
ImuI::getData(const Ice::Current& current) const
{
    std::cout << "getData()" << std::endl;

    // create a null pointer. data will be cloned into it.
    orca::ImuDataPtr data;
    // we don't need to pop the data here because we don't block on it.
    if ( imuDataBuffer_.isEmpty() )
    {
        cout << "ERROR(imuI.cpp): getData() called when no data had been generated!!" << endl;
        throw orca::DataNotExistException( "Imu proxy is not populated yet" );
    }else{
        imuDataBuffer_.get( data );
    }

    return data;
}

// Get Imu Geometry
::orca::ImuDescriptionPtr
ImuI::getDescription(const ::Ice::Current& ) const
{
    std::cout << "getConfig()" << std::endl;
    return descr_;
}

::orca::ImuDescriptionPtr
ImuI::localGetDescription() const
{
    return descr_;
}

// Subscribe people
void 
ImuI::subscribe(const ::orca::ImuConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "subscribe()" << endl;
    IceStorm::QoS qos;
    topicPrx_->subscribe( qos, subscriber );
}

// Unsubscribe people
void 
ImuI::unsubscribe(const ::orca::ImuConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "unsubscribe()" << endl;
    topicPrx_->unsubscribe( subscriber );
}

// Set raw IMU Data
void
ImuI::localSetData( ::orca::ImuDataPtr data )
{
    // Stick it in the buffer so pullers can get it
    imuDataBuffer_.push( data );

    try {
        // push it to IceStorm
        imuPublisher_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down,
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}

