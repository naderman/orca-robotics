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
#include <sstream>

#include "nethandler.h"
#include <IceUtil/Time.h>
#include <IceStorm/IceStorm.h>

// implementations of Ice objects
#include "platform2dI.h"
#include "position3dI.h"
#include "powerI.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;
using namespace segwayrmp;

NetHandler::NetHandler(
                 orcaice::PtrProxy<orca::Position2dDataPtr>    & position2dPipe,
                 orcaice::PtrProxy<orca::Position3dDataPtr>    & position3dPipe,
                 orcaice::PtrNotify<orca::Velocity2dCommandPtr>& commandPipe,
                 orcaice::PtrProxy<orca::PowerDataPtr>         & powerPipe,
                 orcaice::PtrProxy<orca::Platform2dConfigPtr>  & setConfigPipe,
                 orcaice::PtrProxy<orca::Platform2dConfigPtr>  & currentConfigPipe,
                 const orcaice::Context                        & context )
      : position2dPipe_(position2dPipe),
        position3dPipe_(position3dPipe),
        commandPipe_(commandPipe),
        powerPipe_(powerPipe),
        setConfigPipe_(setConfigPipe),
        currentConfigPipe_(currentConfigPipe),
        position2dData_(new Position2dData),
        position3dData_(new Position3dData),
        commandData_(new Velocity2dCommand),
        powerData_(new PowerData),
        context_(context)
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
    position3dPublishInterval_ = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"Position3dPublishInterval", -1 );
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

    // PROVIDED: Position3d
    // Find IceStorm Topic to which we'll publish.
    // NetworkException will kill it, that's what we want.
    IceStorm::TopicPrx pos3dTopicPrx = orcaice::connectToTopicWithTag<Position3dConsumerPrx>
                ( context_, position3dPublisher_, "Position3d" );

    // create servant for direct connections and tell adapter about it
    // don't need to store it as a member variable, adapter will keep it alive
    Ice::ObjectPtr position3dObj = new Position3dI( position3dPipe_, pos3dTopicPrx );
    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context_, position3dObj, "Position3d" );

    // PROVIDED INTERFACE: Power
    // Find IceStorm ConsumerProxy to push out data
    IceStorm::TopicPrx powerTopicPrx = orcaice::connectToTopicWithTag<PowerConsumerPrx>
                ( context_, powerPublisher_, "Power" );
    
    // create servant for direct connections and tell adapter about it
    Ice::ObjectPtr powerObj = new PowerI( powerPipe_, powerTopicPrx );
    orcaice::createInterfaceWithTag( context_, powerObj, "Power" );
    
    // all cool, assume we can send and receive
    context_.tracer()->debug("network enabled",5);
}

void
NetHandler::run()
{
    try // this is once per run try/catch: waiting for the communicator to be destroyed
    {

    int position2dReadTimeout = 1000; // [ms]
    orcaice::Timer pushTimer;

    while ( isActive() )
    {
        try {
            cout<<"activating..."<<endl;
            context_.activate();
            cout<<"activated."<<endl;
            break;
        }
        catch ( orcaice::NetworkException & e )
        {
            std::stringstream ss;
            ss << "nethandler::run: Caught NetworkException: " << e.what() << endl << "Will try again...";
            context_.tracer()->warning( ss.str() );
        }
        catch ( Ice::Exception & e )
        {
            std::stringstream ss;
            ss << "nethandler::run: Caught Ice::Exception while activating: " << e << endl << "Will try again...";
            context_.tracer()->warning( ss.str() );
        }
        catch ( ... )
        {
            cout << "Caught some other exception while activating." << endl;
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
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
            if ( position3dPublishInterval_<0 ||
                        position3dPublishTimer_.elapsed().toSecondsDouble()>position3dPublishInterval_ ) {
                // also check if the data is new
                if ( position3dPipe_.isNewData() ) {
                    position3dPipe_.get( position3dData_ );
                    position3dPublisher_->setData( position3dData_ );
                    position3dPublishTimer_.restart();
                }
            }
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
                context_.status()->heartbeat( context_.status()->status() );
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
            context_.status()->heartbeat("status OK");
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
