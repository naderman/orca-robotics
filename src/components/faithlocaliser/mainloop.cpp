#include <iostream>
#include <orcaice/orcaice.h>

#include "mainloop.h"

using namespace std;
using namespace orca;
using namespace faithlocaliser;
using orcaice::operator<<;

MainLoop::MainLoop( const Localise2dConsumerPrx                    localise2dConsumer,
                    orcaice::PtrBuffer<orca::Position2dDataPtr>   &posBuffer,
                    orcaice::PtrBuffer<Localise2dDataPtr>         &locBuffer,
                    orcaice::Context                               context )
    : localise2dConsumer_(localise2dConsumer),
      posBuffer_(posBuffer),
      locBuffer_(locBuffer),
      context_(context)
{
    assert(localise2dConsumer_ != 0);
}

MainLoop::~MainLoop()
{
}

void
MainLoop::run()
{
    Localise2dDataPtr localiseData = new Localise2dData;
    Position2dDataPtr odomData     = new Position2dData;

    try 
    {
        while ( isActive() )
        {
            // cout<<"============================================="<<endl;

            // Get odometry info
            posBuffer_.getAndPopNext( odomData );

            // Copy the odometry
            localiseData->timeStamp = odomData->timeStamp;
            localiseData->hypotheses.resize(1);
            localiseData->hypotheses[0].weight = 1.0;
            localiseData->hypotheses[0].mean.p.x = odomData->pose.p.x;
            localiseData->hypotheses[0].mean.p.y = odomData->pose.p.y;
            localiseData->hypotheses[0].mean.o   = odomData->pose.o;
            localiseData->hypotheses[0].cov.xx   = 0.01;
            localiseData->hypotheses[0].cov.yy   = 0.01;
            localiseData->hypotheses[0].cov.tt   = 0.01*M_PI/180.0;
            localiseData->hypotheses[0].cov.xy   = 0.0;
            localiseData->hypotheses[0].cov.xt   = 0.0;
            localiseData->hypotheses[0].cov.yt   = 0.0;

            context_.tracer()->debug( orcaice::toString(localiseData), 5 );

            // Stick the new data in the buffer
            locBuffer_.push( localiseData );

            try {
                // push to IceStorm
                localise2dConsumer_->setData( localiseData );
            }
            catch ( Ice::ConnectionRefusedException &e )
            {
                // This could happen if IceStorm dies.
                // If we're running in an IceBox and the IceBox is shutting down, 
                // this is expected (our co-located IceStorm is obviously going down).
                context_.tracer()->warning( "Failed push to IceStorm." );
            }
        }
    }
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop.
        //
        // Could probably handle it better for an Application by stopping the component on Ctrl-C
        // before shutting down communicator.
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();

    cout<<"TRACE(filtermainloop.cpp): Dropping out from run()" << endl;
}
