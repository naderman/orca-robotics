/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "testloop.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;

TestLoop::TestLoop( const StatusConsumerPrx & statusCallbackPrx,
                    const orcaice::Context & context ) :
        statusCallbackPrx_(statusCallbackPrx),
        context_(context)
{
    // seed random number
    srand( (unsigned int)time(0) );
}

TestLoop::~TestLoop()
{
}

void TestLoop::run()
{
    // remote objects
    orca::HomePrx homePrx;
    orca::StatusPrx statusPrx;

    // REQUIRED : Home
    while ( !isStopping() ) {
        try
        {
            // create a proxy for the remote server based on its name in the config file
            orcaice::connectToInterfaceWithTag<HomePrx>( context_, homePrx, "Home" );
            context_.tracer()->info( "connected to a 'Home' interface");
            break;
        }
        catch ( const Ice::ConnectionRefusedException & e )
        {
            context_.tracer()->warning( "failed to connect to a remote interface 'Home'");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }

    // REQUIRED : Status
    while ( !isStopping() ) {
        try
        {
            // create a proxy for the remote server based on its name in the config file
            orcaice::connectToInterfaceWithTag<StatusPrx>( context_, statusPrx, "Status" );
            context_.tracer()->info( "connected to a 'Status' interface");
            break;
        }
        catch ( const Ice::ConnectionRefusedException & e )
        {
            context_.tracer()->warning( "failed to connect to a remote interface 'Status'");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }

    HomeData homeData;
    StatusData statusData;

    while ( !isStopping() ) {
        if ( rand()%1000 > 998 ) {
            try {
                statusPrx->subscribe( statusCallbackPrx_ );
                context_.tracer()->info( "subscribed to 'Status' interface" );
            }
            catch ( const Ice::LocalException & e )  {
                cout<<e<<endl;
            }
        }

        if ( rand()%1000 > 998 ) {
            try {
                statusPrx->unsubscribe( statusCallbackPrx_ );
                context_.tracer()->info( "unsubscribed from 'Status' interface" );
            }
            catch ( const Ice::LocalException & e )  {
                cout<<e<<endl;
            }
        }

        if ( rand()%1000 > 950 ) {
            try {
                statusData = statusPrx->getData();
                context_.tracer()->print( orcaice::toString(statusData) );
            }
            catch ( const Ice::LocalException & e )  {
                cout<<e<<endl;
            }
        }

        if ( rand()%1000 > 990 ) {
            try {
                homeData = homePrx->getInterfaces();
                context_.tracer()->print( orcaice::toString(homeData) );
            }
            catch ( const Ice::LocalException & e )  {
                cout<<e<<endl;
            }
        }

        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(200));
    }

    context_.tracer()->debug("exiting thread",5);
}
