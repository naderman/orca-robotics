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
#include <assert.h>
#include <orcaice/orcaice.h>

#include "algohandler.h"
#include "pathplanner2dconsumerI.h"

using namespace std;
using namespace orca;
using namespace pathplantester;

AlgoHandler::AlgoHandler( const orcaice::Context & context )
    : context_(context)
{
    initNetwork();
    getConfig();
}

AlgoHandler::~AlgoHandler()
{
}

void
AlgoHandler::initNetwork()
{
    //
    // REQUIRED INTERFACE: PathPlanner2d
    //

    while( !isStopping() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::PathPlanner2dPrx>( context_, pathPlanner2dPrx_, "PathPlanner2d" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            context_.tracer()->error( "failed to connect to remote object. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        // NOTE: connectToInterfaceWithTag() can also throw ConfigFileException,
        //       but if this happens it's ok if we just quit.
    }

    //
    // PROVIDED INTERFACE: PathPlanner2dConsumer
    //

    pathPlanner2dConsumerObj_ = new PathPlanner2dConsumerI;
    callbackPrx_ = orcaice::createConsumerInterface<PathPlanner2dConsumerPrx>( context_, pathPlanner2dConsumerObj_ );

}

void
AlgoHandler::getConfig()
{
    //
    // Read in path from file
    //
    std::string prefix = context_.tag() + ".Config.";
    Waypoint2d wp;
    orcaice::setInit(wp);
    
    wp.target.p.x = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"StartWp.x", 0.0 );
    wp.target.p.y = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"StartWp.y", 0.0 );
    coarsePath_.push_back( wp );
    
    for(unsigned int i=0; ; i++)
    {
        stringstream* ss = new stringstream;
        *ss << i;

        if ( (orcaice::getPropertyAsDouble( context_.properties(), prefix+"GoalWp" + ss->str() +".x", wp.target.p.x) != 0 ) ||
             (orcaice::getPropertyAsDouble( context_.properties(), prefix+"GoalWp" + ss->str() +".y", wp.target.p.y) != 0 ) ) 
        break;

        coarsePath_.push_back( wp );
        delete ss;
    }

}


void 
AlgoHandler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {

        PathPlanner2dTask task;
        
        // set task
        task.coarsePath = coarsePath_;
    
        // create a proxy to the pathplan2dconsumer interface which gets passed to the pathplanner via the task
        task.prx = callbackPrx_;
        
        while ( !isStopping() )
        {
            cout << "INFO(algohandler.cpp): Task is:" << endl << orcaice::toVerboseString(task) << endl;
            cout << "INFO(algohandler.cpp): Press button to send task" << endl;
            getchar();
            pathPlanner2dPrx_->setTask( task );
        }

    //
    // unexpected exceptions
    //
    } // try
    catch ( const orca::OrcaException & e )
    {
        context_.tracer()->print( e.what );
        context_.tracer()->error( "unexpected (remote?) orca exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const orcaiceutil::Exception & e )
    {
        context_.tracer()->print( e.what() );
        context_.tracer()->error( "unexpected (local?) orcaice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        cout<<e<<endl;
        context_.tracer()->error( "unexpected Ice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        cout<<e.what()<<endl;
        context_.tracer()->error( "unexpected std exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( ... )
    {
        context_.tracer()->error( "unexpected exception from somewhere.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
}
