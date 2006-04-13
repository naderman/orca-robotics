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
#include <assert.h>
#include <orcaice/orcaice.h>

#include "algohandler.h"
#include "pathplanner2dI.h"
#include "skeletongraphicsI.h"
#include "fakedriver.h"
#include "gridpotentialdriver.h"
#include "astardriver.h"

using namespace std;
using namespace orca;
using namespace pathplanner;
using orcaice::operator<<;

AlgoHandler::AlgoHandler( const orcaice::Context & context )
    : driver_(0),
      pathPlannerTaskProxy_(0),
      context_(context)
{
    initNetwork();
    initDriver();
}

AlgoHandler::~AlgoHandler()
{
    delete pathPlannerTaskProxy_;
}

void
AlgoHandler::initNetwork()
{
    //
    // REQUIRED INTERFACE: OgMap
    //

    while( isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::OgMapPrx>( context_, ogMapPrx_, "OgMap" );
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
    // PROVIDED INTERFACES
    //
    

    // PathPlanner2d
    // create the proxy/buffer for tasks
    pathPlannerTaskProxy_ = new orcaice::PtrProxy<PathPlanner2dTaskPtr>; 
    pathPlannerDataProxy_ = new orcaice::PtrProxy<PathPlanner2dDataPtr>;

    pathPlannerI_ = new PathPlanner2dI( *pathPlannerTaskProxy_, *pathPlannerDataProxy_, context_ );
    Ice::ObjectPtr pathPlannerObj = pathPlannerI_;
    //pathPlanner2dObj_ = new PathPlanner2dI( *pathPlannerTaskProxy_, *pathPlannerDataProxy_ );
    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context_, pathPlannerObj, "PathPlanner2d" );

    // QGraphics2d
    graphicsI_ = new SkeletonGraphicsI( context_, "SkeletonGraphics" );
    Ice::ObjectPtr graphicsObj = graphicsI_;
    orcaice::createInterfaceWithTag( context_, graphicsObj, "SkeletonGraphics" ); 
}

void
AlgoHandler::initDriver()
{

    // get the map once
    try
    {
        ogMapDataPtr_ = ogMapPrx_->getData();
        context_.tracer()->info( "Got OG map:");
        cout << ogMapDataPtr_;
    }
    catch ( const orca::DataNotExistException & e )
    {
        cout<<"warning: "<<e.what<<endl;
    }


    //
    // Read settings
    //
    std::string prefix = context_.tag() + ".Config.";

    orcapathplan::Config config;

    config.traversabilityTreshhold = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"TraversabilityTreshhold", 0.3 );
    config.robotDiameterMetres = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"RobotDiameterMetres", 0.8 );
    config.doPathOptimization = orcaice::getPropertyAsIntWithDefault( context_.properties(), prefix+"DoPathOptimization", 0 );

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( context_.properties(), prefix+"Driver", "simplenav" );
    if ( driverName == "simplenav" )
    {
        context_.tracer()->debug( "loading simplenav driver",3);
        driver_ = new GridPotentialDriver( config, graphicsI_, false );
    }
    else if ( driverName == "skeletonnav" )
    {
        context_.tracer()->debug( "loading skeletonnav driver",3);
        driver_ = new GridPotentialDriver( config, graphicsI_, true );
    }
    else if ( driverName == "astar" )
    {
        context_.tracer()->debug( "loading astar driver",3);
        driver_ = new AStarDriver( config, graphicsI_, true );
    }
    else if ( driverName == "fake" )
    {
        context_.tracer()->debug( "loading fake driver",3);
        driver_ = new FakeDriver( config, graphicsI_ );
    }
    else {
        string errorStr = "Unknown driver type.";
        context_.tracer()->error( errorStr);
        context_.tracer()->info( "Valid driver values are {'simplenav', 'skeletonnav', 'astar', 'fake'}" );
        throw orcaice::Exception( ERROR_INFO, errorStr );
    }

    context_.tracer()->debug("driver instantiated",5);
}

void 
AlgoHandler::run()
{
    assert( driver_ );
    assert( pathPlannerTaskProxy_ );

    // we are in a different thread now, catch all stray exceptions
    try
    {

    PathPlanner2dTaskPtr taskPtr; 
    PathPlanner2dDataPtr pathDataPtr = new PathPlanner2dData;   

    while ( isActive() )
    {
        //
        //  ======== waiting for a task (blocking) =======
        //
        context_.tracer()->info("waiting for a new task");
        
        while ( isActive() )
        {
            int ret = pathPlannerTaskProxy_->getNext( taskPtr, 1000 );
            if ( ret!=0 )
            {
                context_.tracer()->info("waiting for a new task");      
            }
            else
            {
                context_.tracer()->info("task arrived");  
                break;
            }
        }
        
        //
        // ===== tell driver to compute the path ========
        //
        
        // input: ogmap, task; output: path
        context_.tracer()->info("telling driver to compute the path now");
        driver_->computePath( ogMapDataPtr_, taskPtr, pathDataPtr );

        //
        // ======= send result ===============
        //
        context_.tracer()->info("sending off the resulting path");

        // There are three methods to let other components know about the computed path:
        // 1. using the proxy
        taskPtr->prx->setData( pathDataPtr );
        // 2. and 3.: use getData or icestorm
        pathPlannerI_->localSetData( pathDataPtr );

        // resize the pathDataPtr: future tasks might not compute a path successfully and we would resend the old path
        pathDataPtr->path.resize( 0 );

    }

    //
    // unexpected exceptions
    //
    } // try
    catch ( const IceUtil::NullHandleException & )
    {
        context_.tracer()->error( "Null Handle Exception. Application was probably Ctrl+C'd");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is a stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const orca::OrcaException & e )
    {
        context_.tracer()->print( e.what );
        context_.tracer()->error( "unexpected (remote?) orca exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is a stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const orcaice::Exception & e )
    {
        context_.tracer()->print( e.what() );
        context_.tracer()->error( "unexpected (local?) orcaice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is a stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        cout<<e<<endl;
        context_.tracer()->error( "unexpected Ice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is a stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        cout<<e.what()<<endl;
        context_.tracer()->error( "unexpected std exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is a stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( ... )
    {
        context_.tracer()->error( "unexpected exception from somewhere.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is a stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    
    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
}
