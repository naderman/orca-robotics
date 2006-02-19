/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>
#include <sstream>

#include "nethandler.h"
#include <IceUtil/Time.h>
#include <IceStorm/IceStorm.h>

// implementations of Ice objects
#include "platform2dI.h"
#include "powerI.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;

NetHandler::NetHandler(
                 orcaice::PtrBuffer<orca::Position2dDataPtr>    & position2dBuffer,
                 orcaice::PtrNotify                             & commandNotify,
                 orcaice::PtrBuffer<orca::PowerDataPtr>         & powerBuffer,
                 orcaice::PtrBuffer<orca::Platform2dConfigPtr>  & setConfigBuffer,
                 orcaice::PtrBuffer<orca::Platform2dConfigPtr>  & currentConfigBuffer,
                 const orcaice::Context                         & context ) :
        position2dBuffer_(position2dBuffer),
        commandNotify_(commandNotify),
        powerBuffer_(powerBuffer),
        setConfigBuffer_(setConfigBuffer),
        currentConfigBuffer_(currentConfigBuffer),
        position2dData_(new Position2dData),
        commandData_(new Velocity2dCommand),
        powerData_(new PowerData),
        context_(context),
        receiveStatus_(-1),
        sendStatus_(-1)
{
    init();
}

NetHandler::~NetHandler()
{
}

void NetHandler::init()
{
    //
    // Read settings
    //
    position2dPublishInterval_ = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            "SegwayRmp.Config.Position2dPublishInterval", -1 );
    powerPublishInterval_ = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            "SegwayRmp.Config.PowerPublishInterval", 20.0 );
    statusPublishInterval_ = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            "SegwayRmp.Config.StatusPublishInterval", 60.0 );

    //
    // EXTERNAL INTERFACES
    //
    // PROVIDED: Platform2d
    // Find IceStorm Topic to which we'll publish.
    // NetworkException will kill it, that's what we want.
    IceStorm::TopicPrx platfTopicPrx = orcaice::connectToTopicWithTag<Position2dConsumerPrx>
                ( context_, position2dPublisher_, "Platform2d" );

    // create servant for direct connections and tell adapter about it
    // don't need to store it as a member variable, adapter will keep it alive
    Ice::ObjectPtr platform2dObj = new Platform2dI( position2dBuffer_, commandNotify_,
                                      setConfigBuffer_, currentConfigBuffer_, platfTopicPrx );
    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context_, platform2dObj, "Platform2d" );

    // PROVIDED INTERFACE: Power
    // Find IceStorm ConsumerProxy to push out data
    IceStorm::TopicPrx powerTopicPrx = orcaice::connectToTopicWithTag<PowerConsumerPrx>
                ( context_, powerPublisher_, "Power" );
    
    // create servant for direct connections and tell adapter about it
    Ice::ObjectPtr powerObj = new PowerI( powerBuffer_, powerTopicPrx );
    orcaice::createInterfaceWithTag( context_, powerObj, "Power" );
    
    // all cool, assume we can send and receive
    receiveStatus_ = 0;
    sendStatus_ = 0;
    context_.tracer()->debug("network enabled",5);
}

void NetHandler::run()
{
    int position2dReadTimeout = 1000; // [ms]
    
    try // this is once per run try/catch: waiting for the communicator to be destroyed
    {
    
    while( isActive() )
    {
        // block on the most frequent data source: position
        int ret = position2dBuffer_.getNext( position2dData_, position2dReadTimeout );
        
        // it's time to publish if we publish every data point or enough time has elapsed
        bool isTimeToPublishPosition2d = position2dPublishInterval_ < 0
                || position2dPublishTimer_.elapsed().toSecondsDouble()>position2dPublishInterval_;
        if ( ret == 0 && isTimeToPublishPosition2d ) {
            // managed to read new data and it's time to publish
            try
            {
                position2dPublisher_->setData( position2dData_ );
                position2dPublishTimer_.restart();
            }
                catch ( const Ice::ConnectionRefusedException & e )
            {
                context_.tracer()->warning("lost connection to IceStorm");
                // now what?
            }
        }

        // now send less frequent updates
        try
        {
            if ( powerPublishInterval_<0 ||
                        powerPublishTimer_.elapsed().toSecondsDouble()>powerPublishInterval_ ) {
                powerBuffer_.get( powerData_ );
                powerPublisher_->setData( powerData_ );
                powerPublishTimer_.restart();
            }
            if ( statusPublishInterval_<0 ||
                        statusPublishTimer_.elapsed().toSecondsDouble()>statusPublishInterval_ ) {
                //cout<<"sending heartbeat"<<endl;
                context_.tracer()->heartbeat("status OK");
                statusPublishTimer_.restart();
            }
        }
        catch ( const Ice::ConnectionRefusedException & e )
        {
            context_.tracer()->warning("lost connection to IceStorm");
            // now what?
        }
            
    } // while
    
    }
    catch ( const Ice::CommunicatorDestroyedException & e )
    {
        // it's ok, we must be quitting.
    }
}

void NetHandler::send()
{
    // push data to IceStorm
    try
    {
        if ( position2dPublishTimer_.elapsed().toSecondsDouble()>position2dPublishInterval_ ) {
            // check that there's new data
            position2dBuffer_.get( position2dData_ );
            position2dPublisher_->setData( position2dData_ );
            position2dPublishTimer_.restart();
        }
        if ( powerPublishTimer_.elapsed().toSecondsDouble()>powerPublishInterval_ ) {
            powerBuffer_.get( powerData_ );
            powerPublisher_->setData( powerData_ );
            powerPublishTimer_.restart();
        }
        if ( statusPublishTimer_.elapsed().toSecondsDouble()>statusPublishInterval_ ) {
            //cout<<"sending heartbeat"<<endl;
            context_.tracer()->heartbeat("status OK");
            statusPublishTimer_.restart();
        }
    }
    catch ( const Ice::ConnectionRefusedException & e )
    {
        context_.tracer()->warning("lost connection to IceStorm");
    }
    catch ( const Ice::CommunicatorDestroyedException & e )
    {
        // it's ok, the communicator may already be destroyed
    }
}
