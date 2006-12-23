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
#include <orcaice/orcaice.h>

#include "mainloop.h"

using namespace std;
using namespace orca;
using namespace faithlocaliser;

MainLoop::MainLoop( const Localise2dConsumerPrx                    localise2dConsumer,
                    orcaice::Buffer<orca::Position2dData>   &posBuffer,
                    orcaice::Buffer<orca::Localise2dData>         &locBuffer,
                    orcaice::Buffer<orca::Localise2dData>         &historyBuffer,
		    double                                         stdDevPosition,
		    double                                         stdDevHeading,
		    const orcaice::Context & context)

    : localise2dConsumer_(localise2dConsumer),
      posBuffer_(posBuffer),
      locBuffer_(locBuffer),
      historyBuffer_(historyBuffer),
      context_(context),
      stdDevPosition_(stdDevPosition),
      stdDevHeading_(stdDevHeading)
{
    assert(localise2dConsumer_ != 0);
}

MainLoop::~MainLoop()
{
}

void
MainLoop::run()
{
    Localise2dData localiseData;
    Position2dData odomData;

    try 
    {
	double varPosition = stdDevPosition_*stdDevPosition_;
        double varHeading = (stdDevHeading_*M_PI/180.0)*(stdDevHeading_*M_PI/180.0);
        while ( isActive() )
        {
            // cout<<"============================================="<<endl;

            // Get odometry info, time out every so often to check if we are cancelled
            if ( posBuffer_.getAndPopNext( odomData, 200 ) != 0 ) {
                // no new value
                continue;
            }

            // Copy the odometry
            localiseData.timeStamp = odomData.timeStamp;
            localiseData.hypotheses.resize(1);
            localiseData.hypotheses[0].weight = 1.0;
            localiseData.hypotheses[0].mean.p.x = odomData.pose.p.x;
            localiseData.hypotheses[0].mean.p.y = odomData.pose.p.y;
            localiseData.hypotheses[0].mean.o   = odomData.pose.o;
            localiseData.hypotheses[0].cov.xx   = varPosition;
            localiseData.hypotheses[0].cov.yy   = varPosition;
            localiseData.hypotheses[0].cov.tt   = varHeading;
            localiseData.hypotheses[0].cov.xy   = 0.0;
            localiseData.hypotheses[0].cov.xt   = 0.0;
            localiseData.hypotheses[0].cov.yt   = 0.0;

            context_.tracer()->debug( orcaice::toString(localiseData), 5 );

            // Stick the new data in the buffer
            locBuffer_.push( localiseData );
	    historyBuffer_.push( localiseData );

	    Localise2dData Data0;

            /*cout << "contents of history\n";
            for(int i=0;i<historyBuffer_.size();i++){
                historyBuffer_.get(Data0,i);
		cout << "time: " << orcaice::timeAsDouble(Data0->timeStamp) << endl;
            }*/

            try {
                // push to IceStorm
                localise2dConsumer_->setData( localiseData );
            }
            catch ( Ice::ConnectionRefusedException &e )
            {
                // This could happen if IceStorm dies.
                // If we're running in an IceBox and the IceBox is shutting down, 
                // this is expected (our co-located IceStorm is obviously going down).
                context_.tracer()->warning( "Failed to push to IceStorm." );
            }
        }
    }
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop.
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
}
