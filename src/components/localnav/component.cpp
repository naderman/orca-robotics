/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "component.h"
#include "mainloop.h"
#include "localnavdriver.h"
#include "localnavmanager.h"
#include "vfhdriver/vfhdriver.h"
#include "pathmaintainer.h"
#include "pathfollower2dI.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;

namespace localnav {

Component::Component()
    : orcaice::Component( "LocalNav" )
{
}

Component::~Component()
{
    if ( platform2dPrx_ != NULL )   delete platform2dPrx_;
    if ( localNavDriver_ != NULL )  delete localNavDriver_;
    if ( localNavManager_ != NULL ) delete localNavManager_;
    if ( pathMaintainer_ != NULL )  delete pathMaintainer_;
    if ( mainLoop_ != NULL )        delete mainLoop_;
}

void
Component::start()
{
    cout<<"TRACE(localnavcomponent.cpp): start()" << endl;
    //
    // INITIAL CONFIGURATION
    //

    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag();
    prefix += ".Config.";

    std::string driverName = orcaice::getPropertyWithDefault( prop, prefix+"Driver", "vfhdriver" );

//     if ( driverName == "test" )
//     {
//         cout<<"TRACE(localnavcomponent.cpp): Instantiating test driver" << endl;
//         LocalNavParams initialNavParams;
//         localNavDriver_ = new VfhDriver( initialNavParams );
//     }
    if ( driverName == "vfhdriver" )
    {
        // Find IceStorm Topic to which we'll publish
        IceStorm::TopicPrx topicPrx = orcaice::connectToTopicWithTag<PathFollower2dConsumerPrx>
            ( context(), pathPublisher_, "PathFollower2d" );

        // connect to the platform
        platform2dPrx_ = new orca::Platform2dPrx;
        orcaice::connectToInterfaceWithTag<Platform2dPrx>( context(), *platform2dPrx_, "Platform2d" );
        context().tracer()->debug("connected to a 'Platform2d' interface",5);

        // Instantiate everything
        obsConsumer_  = new orcaice::PtrBufferedConsumerI<orca::RangeScanner2dConsumer,orca::RangeScanner2dDataPtr>;
        locConsumer_  = new orcaice::BufferedConsumerI<orca::Localise2dConsumer,orca::Localise2dData>;
        odomConsumer_ = new orcaice::BufferedConsumerI<orca::Position2dConsumer,orca::Position2dData>;

        pathMaintainer_ = new PathMaintainer(pathPipe_,
                                             newPathArrivedPipe_,
                                             activationPipe_,
                                             wpIndexPipe_,
                                             context());

        cout<<"TRACE(component.cpp): Initialising driver" << endl;
        localNavDriver_  = new vfh::VfhDriver( sharedGoalWatcher_, context() );
        cout<<"TRACE(component.cpp): Finished initialising driver." << endl;
        localNavManager_ = new LocalNavManager( *localNavDriver_, sharedGoalWatcher_, *pathMaintainer_, context() );
        mainLoop_        = new MainLoop( *localNavManager_,
                                          obsConsumer_->buffer_,
                                          locConsumer_->buffer_,
                                          odomConsumer_->buffer_,
                                          enabledPipe_,
                                         *platform2dPrx_,
                                         *pathMaintainer_,
                                          pathPublisher_,
                                          context() );

        // subscribe for information from platform
        RangeScanner2dPrx obsPrx;
        Localise2dPrx   locPrx;
        Position2dPrx   odomPrx;
        orcaice::connectToInterfaceWithTag<orca::RangeScanner2dPrx>( context(), obsPrx, "Observations" );
        orcaice::connectToInterfaceWithTag<orca::Localise2dPrx>( context(), locPrx, "Localisation" );
        orcaice::connectToInterfaceWithTag<orca::Position2dPrx>( context(), odomPrx, "Platform2d" );

        Ice::ObjectPtr obsConsumerPtr = obsConsumer_;
        orca::RangeScanner2dConsumerPrx obsConsumerPrx =
            orcaice::createConsumerInterface<RangeScanner2dConsumerPrx>( context(), obsConsumerPtr );
        Ice::ObjectPtr locConsumerPtr = locConsumer_;
        orca::Localise2dConsumerPrx locConsumerPrx =
            orcaice::createConsumerInterface<Localise2dConsumerPrx>( context(), locConsumerPtr );
        Ice::ObjectPtr odomConsumerPtr = odomConsumer_;
        orca::Position2dConsumerPrx odomConsumerPrx =
            orcaice::createConsumerInterface<Position2dConsumerPrx>( context(), odomConsumerPtr );

        obsPrx->subscribe(  obsConsumerPrx );
        locPrx->subscribe(  locConsumerPrx );
        odomPrx->subscribe( odomConsumerPrx );

        // create servant for direct connections
        // don't need to store it as a member variable, adapter will keep it alive
        Ice::ObjectPtr pathFollower2dObj = new PathFollower2dI( pathPipe_,
                                                                newPathArrivedPipe_,
                                                                activationPipe_,
                                                                wpIndexPipe_,
                                                                enabledPipe_,
                                                                topicPrx );
        orcaice::createInterfaceWithTag( context(), pathFollower2dObj, "PathFollower2d" );
    }
    else
    {
        std::string errString = "Unknown driver type: "+driverName;
        throw orcaice::Exception( ERROR_INFO, errString );
    }

    ////////////////////////////////////////////////////////////////////////////////

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    cout<<"TRACE(localnavcomponent.cpp): entering main loop..." << endl;

    //
    // MAIN DRIVER LOOP
    //

    mainLoop_->start();    
}

void Component::stop()
{
    orcaice::Thread::stopAndJoin( mainLoop_ );
}

}
