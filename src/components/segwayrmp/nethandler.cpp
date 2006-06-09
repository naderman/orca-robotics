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
using namespace segwayrmp;

NetHandler::NetHandler(
                 orcaice::PtrProxy<orca::Position2dDataPtr>    & position2dPipe,
                 orcaice::PtrNotify<orca::Velocity2dCommandPtr>& commandPipe,
                 orcaice::PtrProxy<orca::PowerDataPtr>         & powerPipe,
                 orcaice::PtrProxy<orca::Platform2dConfigPtr>  & setConfigPipe,
                 orcaice::PtrProxy<orca::Platform2dConfigPtr>  & currentConfigPipe,
                 const orcaice::Context                        & context )
      : position2dPipe_(position2dPipe),
        commandPipe_(commandPipe),
        powerPipe_(powerPipe),
        setConfigPipe_(setConfigPipe),
        currentConfigPipe_(currentConfigPipe),
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

void
NetHandler::init()
{
    //
    // Read settings
    //
    std::string prefix = context_.tag() + ".Config.";
    
    position2dPublishInterval_ = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"Position2dPublishInterval", -1 );
    powerPublishInterval_ = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"PowerPublishInterval", 20.0 );
    statusPublishInterval_ = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"StatusPublishInterval", 60.0 );

    // PROVIDED: Platform2d
    // Find IceStorm Topic to which we'll publish.
    // NetworkException will kill it, that's what we want.
    IceStorm::TopicPrx platfTopicPrx = orcaice::connectToTopicWithTag<Position2dConsumerPrx>
                ( context_, position2dPublisher_, "Platform2d" );

    // create servant for direct connections and tell adapter about it
    // don't need to store it as a member variable, adapter will keep it alive
    Ice::ObjectPtr platform2dObj = new Platform2dI( position2dPipe_, commandPipe_,
                                      setConfigPipe_, currentConfigPipe_, platfTopicPrx );
    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context_, platform2dObj, "Platform2d" );

    // PROVIDED INTERFACE: Power
    // Find IceStorm ConsumerProxy to push out data
    IceStorm::TopicPrx powerTopicPrx = orcaice::connectToTopicWithTag<PowerConsumerPrx>
                ( context_, powerPublisher_, "Power" );
    
    // create servant for direct connections and tell adapter about it
    Ice::ObjectPtr powerObj = new PowerI( powerPipe_, powerTopicPrx );
    orcaice::createInterfaceWithTag( context_, powerObj, "Power" );
    
    // all cool, assume we can send and receive
    receiveStatus_ = 0;
    sendStatus_ = 0;
    context_.tracer()->debug("network enabled",5);
}

void
NetHandler::run()
{
    try // this is once per run try/catch: waiting for the communicator to be destroyed
    {

    int position2dReadTimeout = 1000; // [ms]
    orcaice::Timer pushTimer;

    int activateRetryNumber = context_.properties()->getPropertyAsInt( "Orca.ActivateRetryNumber" );
    int count = -1; // this retry count, not try count
    while ( isActive() && ( activateRetryNumber<0 || count<activateRetryNumber ) )
    {
        try {
            cout<<"activating..."<<endl;
            context_.activate();
            cout<<"activated."<<endl;
            break;
        }
        catch ( orcaice::NetworkException & e )
        {
            cout<<e.what()<<endl;
            cout << "Will try again..." << endl;
        }
        catch ( Ice::Exception & e )
        {
            cout<<e<<endl;
            cout << "Caught some Ice exception while activating.  Will try again..." << endl;
        }
        catch ( ... )
        {
            cout << "Caught some other exception while activating." << endl;
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        ++count;
    }
    
    while( isActive() )
    {
        // block on the most frequent data source: position
        int ret = position2dPipe_.getNext( position2dData_, position2dReadTimeout );
        
        // it's time to publish if we publish every data point or enough time has elapsed
        bool isTimeToPublishPosition2d = position2dPublishInterval_ < 0
                || position2dPublishTimer_.elapsed().toSecondsDouble()>position2dPublishInterval_;
        if ( ret == 0 && isTimeToPublishPosition2d )
        {
//debug
//cout<<"push: " << pushTimer.elapsed().toMilliSecondsDouble()<<endl;
//pushTimer.restart();

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
                // also check if the data is new
                if ( powerPipe_.isNewData() ) {
                    powerPipe_.get( powerData_ );
                    powerPublisher_->setData( powerData_ );
                    powerPublishTimer_.restart();
                }
            }
            // todo: the logic of this needs revisiting
            if ( statusPublishInterval_<0 ||
                        statusPublishTimer_.elapsed().toSecondsDouble()>statusPublishInterval_ ) {
                //cout<<"sending heartbeat"<<endl;
                context_.tracer()->heartbeat( context_.tracer()->status() );
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

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "exiting NetHandler thread...",5);
}

void
NetHandler::send()
{
    // push data to IceStorm
    try
    {
        if ( position2dPublishTimer_.elapsed().toSecondsDouble()>position2dPublishInterval_ ) {
            // check that there's new data
            position2dPipe_.get( position2dData_ );
            position2dPublisher_->setData( position2dData_ );
            position2dPublishTimer_.restart();
        }
        if ( powerPublishTimer_.elapsed().toSecondsDouble()>powerPublishInterval_ ) {
            powerPipe_.get( powerData_ );
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
