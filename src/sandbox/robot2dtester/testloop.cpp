/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "testloop.h"

using namespace std;
using namespace orca;

TestLoop::TestLoop( const Odometry2dConsumerPrx odometry2dCallbackPrx,
                    const orca::PowerConsumerPrx powerCallbackPrx,
                    const orcaice::Context & context ) :
        odometry2dCallbackPrx_(odometry2dCallbackPrx),
        powerCallbackPrx_(powerCallbackPrx),
        context_(context)
{
    // seed random number
    srand( (unsigned int)time(0) );
}

TestLoop::~TestLoop()
{
}

void
TestLoop::printEvents()
{
    for ( unsigned int i=0; i < events_.size(); i++ )
    {
        cout << "Event " << i << ": " << events_[i] << endl;
    }
}

void
TestLoop::connectToPos( orca::Platform2dPrx &platform2dPrx )
{
    while ( isActive() ) {
        try
        {
            orcaice::connectToInterfaceWithTag<Platform2dPrx>( context_, platform2dPrx, "Platform2d" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            context_.tracer()->error( "failed to connect. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        catch ( std::exception &e )
        {
            cout << "ERROR(testloop.cpp): " << "TestLoop::connectToPos: " << e.what() << endl;
            throw;
        }
        catch ( ... )
        {
            cout << "ERROR(testloop.cpp): TestLoop::connectToPos: unknown exception." << endl;
            throw;
        }
    }
}

void
TestLoop::connectToPower( orca::PowerPrx &powerPrx )
{
    while ( isActive() ) {
        try
        {
            // create a proxy for the remote server based on its name in the config file
            orcaice::connectToInterfaceWithTag<PowerPrx>( context_, powerPrx, "Power" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            context_.tracer()->error( "failed to connect. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        catch ( std::exception &e )
        {
            cout << "ERROR(testloop.cpp): " << "TestLoop::connectToPower: " << e.what() << endl;
            throw;
        }
        catch ( ... )
        {
            cout << "ERROR(testloop.cpp): TestLoop::connectToPower: unknown exception." << endl;
            throw;
        }
    }
}

// read commands from the keyboard
void 
TestLoop::run()
{
    // catch all stray exceptions
    try {

        // REQUIRED : Odometry2d
        orca::Platform2dPrx platform2dPrx;
        connectToPos( platform2dPrx );
    
        // REQUIRED : Power
        orca::PowerPrx powerPrx;
        connectToPower( powerPrx );
    
        Odometry2dData odometry2dData;
        PowerData powerData;
        Velocity2dCommand velocity2dCommand;
        velocity2dCommand.motion.v.y = 0.0;
    
        int windUpCounter = 0;

        while ( isActive() ) {

            try {
            
                if ( rand()%1000 > 998 ) {
                    platform2dPrx->subscribe( odometry2dCallbackPrx_ );
                    context_.tracer()->print( "subscribed to 'Odometry2d' interface" );
                }

                if ( rand()%1000 > 998 ) {
                    platform2dPrx->unsubscribe( odometry2dCallbackPrx_ );
                    context_.tracer()->print( "unsubscribed from 'Odometry2d' interface" );
                }

                if ( rand()%1000 > 950 ) {
                    odometry2dData = platform2dPrx->getData();
                    context_.tracer()->print( orcaice::toString(odometry2dData) );
                }

                ++windUpCounter;
                if ( rand()%1000 > 950 ) {
                    if ( rand()%100 > 50 ) {
                        velocity2dCommand.motion.v.x += 0.05;
                    }
                    else {
                        velocity2dCommand.motion.v.x -= 0.05;
                    }
                    // no turns for now
                    velocity2dCommand.motion.w= 0.0;
                    context_.tracer()->print( "changed motion command. "+orcaice::toString(velocity2dCommand) );
                
                    if ( windUpCounter>10000 ) {
                        velocity2dCommand.motion.v.x = 0.0;
                        velocity2dCommand.motion.v.x = 0.0;
                        windUpCounter = 0;
                        printEvents();
                    }
                }
                platform2dPrx->setCommand( velocity2dCommand );
            
                // POWER INTERFACE 
                if ( rand()%1000 > 998 ) {
                    powerPrx->subscribe( powerCallbackPrx_ );
                    context_.tracer()->print( "subscribed to 'Power' interface" );
                }
            
                if ( rand()%1000 > 998 ) {
                    powerPrx->unsubscribe( powerCallbackPrx_ );
                    context_.tracer()->print( "unsubscribed from 'Power' interface" );
                }
            
                if ( rand()%1000 > 950 ) {
                    powerData = powerPrx->getData();
                    context_.tracer()->print( orcaice::toString(powerData) );
                }
            
                IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(50));
            }
            // Catch and log all exceptions.  Keep going with non-fatal ones.
            catch ( const Ice::CommunicatorDestroyedException & e )
            {
                cout<<"TRACE(testloop.cpp): caught CommunicatorDestroyedException" << endl;
                throw;
            }
            catch ( const orca::DataNotExistException & e )
            {
                stringstream ss;
                ss << orcaice::toString( orcaice::getNow() ) 
                   << "TestLoop: Caught DataNotExistException: " << e.what;
                context_.tracer()->warning( ss.str() );
                events_.push_back( ss.str() );

                // I guess this might happen under normal conditions -- keep going
            }
            catch ( const orca::HardwareFailedException & e )
            {
                stringstream ss;
                ss << orcaice::toString( orcaice::getNow() ) 
                   << "TestLoop: Caught HardwareFailedException: " << e.what;
                context_.tracer()->warning( ss.str() );
                events_.push_back( ss.str() );

                // We wanna know about this. re-throw.
                throw;
            }
            // Anything else is probably normal...
            catch ( std::exception &e )
            {
                stringstream ss;
                ss << orcaice::toString( orcaice::getNow() ) 
                   << "TestLoop: Caught " << e.what();
                context_.tracer()->warning( ss.str() );
                events_.push_back( ss.str() );
            }
            catch ( Ice::Exception &e )
            {
                stringstream ss;
                ss << orcaice::toString( orcaice::getNow() ) 
                   << "TestLoop: Caught " << e;
                context_.tracer()->warning( ss.str() );
                events_.push_back( ss.str() );
            }
            catch ( ... )
            {
                stringstream ss;
                ss << orcaice::toString( orcaice::getNow() ) 
                   << "TestLoop: Caught unknown exception.";
                context_.tracer()->warning( ss.str() );
                events_.push_back( ss.str() );
            }
        } // end while
    }
    catch ( Ice::Exception &e )
    {
        cout<<"TRACE(testloop.cpp): Quitting with exception: " << e << endl;
        printEvents();
    }
    catch ( std::exception &e )
    {
        cout<<"TRACE(testloop.cpp): Quitting with exception: " << e.what() << endl;
        printEvents();
    }
    catch ( ... )
    {
        cout<<"TRACE(testloop.cpp): Quitting with unknown exception." << endl;
        printEvents();
    }
}
