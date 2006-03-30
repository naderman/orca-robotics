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
using orcaice::operator<<;

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

    Ice::PropertiesPtr prop = communicator()->getProperties();
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
        obsConsumer_  = new orcaice::BufferedConsumerI<orca::RangeScannerConsumer,orca::RangeScannerDataPtr>;
        locConsumer_  = new orcaice::BufferedConsumerI<orca::Localise2dConsumer,orca::Localise2dDataPtr>;
        odomConsumer_ = new orcaice::BufferedConsumerI<orca::Position2dConsumer,orca::Position2dDataPtr>;

        pathMaintainer_ = new PathMaintainer(pathPipe_,
                                             newPathArrivedPipe_,
                                             activationPipe_,
                                             wpIndexPipe_,
                                             context());

        cout<<"TRACE(component.cpp): Initialising driver" << endl;
        localNavDriver_  = new VfhDriver( sharedGoalWatcher_, context() );
        cout<<"TRACE(component.cpp): Finished initialising driver." << endl;
        localNavManager_ = new LocalNavManager( *localNavDriver_, sharedGoalWatcher_, *pathMaintainer_, context() );
        mainLoop_        = new MainLoop( *localNavManager_,
                                          obsConsumer_->buffer_,
                                          locConsumer_->buffer_,
                                          odomConsumer_->buffer_,
                                         *platform2dPrx_,
                                         *pathMaintainer_,
                                          pathPublisher_,
                                          context() );

        // subscribe for information from platform
        RangeScannerPrx obsPrx;
        Localise2dPrx   locPrx;
        Position2dPrx   odomPrx;
        orcaice::connectToInterfaceWithTag<orca::RangeScannerPrx>( context(), obsPrx, "Observations" );
        orcaice::connectToInterfaceWithTag<orca::Localise2dPrx>( context(), locPrx, "Localisation" );
        orcaice::connectToInterfaceWithTag<orca::Position2dPrx>( context(), odomPrx, "Platform2d" );

        Ice::ObjectPtr obsConsumerPtr = obsConsumer_;
        orca::RangeScannerConsumerPrx obsConsumerPrx =
            orcaice::createConsumerInterface<RangeScannerConsumerPrx>( context(), obsConsumerPtr );
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
