#include <orcaice/orcaice.h>

#include "component.h"
#include "mainloop.h"
// implementations of Ice objects
#include "localise2d_i.h"

using namespace std;
using namespace orca;
using namespace faithlocaliser;
using orcaice::operator<<;

Component::Component()
    : orcaice::Component( "FaithLocaliser" ),
      mainLoop_(0)
{
}

Component::~Component()
{
}

void
Component::start()
{
    cout<<"TRACE(component.cpp): start()" << endl;
    //
    // INITIAL CONFIGURATION
    //

    Ice::PropertiesPtr prop = communicator()->getProperties();
    std::string prefix = tag();
    prefix += ".Config.";

    //
    // EXTERNAL PROVIDED INTERFACE
    //
    // Find IceStorm Topic to which we'll publish
    IceStorm::TopicPrx localiseTopicPrx = orcaice::connectToTopicWithTag<Localise2dConsumerPrx>
        ( context(), localise2dPublisher_, "Localise2d" );
    
    // create servant for direct connections
    localise2dObj_ = new Localise2dI( localiseTopicPrx, locBuffer_ );
    orcaice::createInterfaceWithTag( context(), localise2dObj_, "Localise2d" );

    ////////////////////////////////////////////////////////////////////////////////

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    //
    // EXTERNAL REQUIRED INTERFACES
    //
    Position2dPrx pos2dPrx;
    orcaice::connectToInterfaceWithTag<orca::Position2dPrx>( context(),
                                                             pos2dPrx,
                                                             "Odometry" );

    posConsumer_ = new orcaice::BufferedConsumerI<orca::Position2dConsumer,orca::Position2dDataPtr>;
    Ice::ObjectPtr consumerPtr = posConsumer_;
    orca::Position2dConsumerPrx consumerPrx =
        orcaice::createConsumerInterface<Position2dConsumerPrx>( context(), consumerPtr );
    pos2dPrx->subscribe( consumerPrx );

    ////////////////////////////////////////////////////////////////////////////////

    cout<<"TRACE(component.cpp): entering main loop..." << endl;

    //
    // MAIN DRIVER LOOP
    //

    mainLoop_ = new MainLoop(  localise2dPublisher_,
                               posConsumer_->buffer_,
                               locBuffer_,
                               context() );
    
    mainLoop_->start();    
}

void
Component::stop()
{
    // Tell the main loop to stop
    mainLoop_->stop();

    IceUtil::ThreadControl tc = mainLoop_->getThreadControl();

    // Then wait for it
    tc.join();
}
