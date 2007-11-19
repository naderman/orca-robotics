/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "skeletondriver.h"
#include <orcapathplan/skeletonpathplanner.h>
#include <orcapathplan/sparseskeletonpathplanner.h>
#include <hydroutil/cpustopwatch.h>
#include <orcapathplan/sparseskel.h>
#include <orcaice/orcaice.h>
#include <iostream>

#ifdef QT4_FOUND
    #include "skeletongraphicsI.h"
#endif

using namespace std;
using namespace orcaice;

namespace pathplanner {
    
SkeletonDriver::SkeletonDriver( const hydroogmap::OgMap &ogMap,
                                double robotDiameterMetres,
                                double traversabilityThreshhold,
                                bool   doPathOptimization,
                                bool   jiggleWaypointsOntoClearCells,
                                bool   useSparseSkeleton,
                                const orcapathplan::CostEvaluator &costEvaluator,
                                const Context &context)
    : pathPlanner_(NULL),
      genericDriver_(NULL),
      skelGraphicsI_(NULL),
      context_(context),
      useSparseSkeleton_(useSparseSkeleton)
      
{
    context_.tracer()->debug("SkeletonDriver: constructor",3);
    
    hydroutil::CpuStopwatch watch(true);
    if ( !useSparseSkeleton )
    {
        try {
            orcapathplan::SkeletonPathPlanner *skelPathPlanner = 
                new orcapathplan::SkeletonPathPlanner( ogMap,
                                                       robotDiameterMetres,
                                                       traversabilityThreshhold,
                                                       doPathOptimization,
                                                       costEvaluator );
            pathPlanner_ = skelPathPlanner;
        }
        catch ( orcapathplan::Exception &e )
        {
            stringstream ss;
            ss << "Error trying to construct a skeletonpathplanner: " << e.what();
            throw orcapathplan::Exception( ss.str(), e.type() );
        } 

    }
    else
    {
        try {
            orcapathplan::SparseSkeletonPathPlanner *skelPathPlanner = 
                new orcapathplan::SparseSkeletonPathPlanner( ogMap,
                                                             robotDiameterMetres,
                                                             traversabilityThreshhold,
                                                             doPathOptimization,
                                                             costEvaluator );
            pathPlanner_ = skelPathPlanner;
        }
        catch( orcapathplan::Exception &e )
        {
            stringstream ss;
            ss << "Error trying to construct a sparseskeletonpathplanner: " << e.what();
            throw orcapathplan::Exception( ss.str(), e.type() );
        }  
        catch ( Ice::MemoryLimitException &e )
        {
            stringstream ss;
            ss<<"Caught: " << e << " .This means the dense skel is too big to send out." << endl;
            throw orcapathplan::Exception( ss.str() );
        }
    }
    
    stringstream ss;
    ss << "Creating skeleton took " << watch.elapsedSeconds() << "s";
    context_.tracer()->debug(ss.str(),1);
        
    genericDriver_ = new GenericDriver( pathPlanner_,
                                        ogMap,
                                        robotDiameterMetres,
                                        traversabilityThreshhold,
                                        jiggleWaypointsOntoClearCells,
                                        doPathOptimization,
                                        context );
    
    #ifdef QT4_FOUND
    bool provideSkeletonGraphics = orcaice::getPropertyAsDoubleWithDefault( context.properties(), context.tag() + ".Config.Skeleton.ProvideGraphics", 1 );
    if ( provideSkeletonGraphics )
    {
        // QGraphics2d
        context.tracer()->info( "Instantiating QGraphics2d Interface" );
        skelGraphicsI_ = new SkeletonGraphicsI( context, "SkeletonGraphics" );
        Ice::ObjectPtr graphicsObj = skelGraphicsI_;
        orcaice::createInterfaceWithTag( context, graphicsObj, "SkeletonGraphics" ); 
        setGraphics(  ogMap, 
                      robotDiameterMetres, 
                      traversabilityThreshhold, 
                      doPathOptimization );
    }
    #endif
                                       
    
}

SkeletonDriver::~SkeletonDriver()
{
    if (pathPlanner_!=NULL) delete pathPlanner_;
    if (genericDriver_!=NULL) delete genericDriver_;
}

#ifdef QT4_FOUND
void
SkeletonDriver::setGraphics( const hydroogmap::OgMap &ogMap,
                             double            robotDiameterMetres,
                             double            traversabilityThreshhold,
                             bool              doPathOptimization )
{                   
    if ( !useSparseSkeleton_ )
    {
        orcapathplan::SkeletonPathPlanner *skelPathPlanner = dynamic_cast<orcapathplan::SkeletonPathPlanner*>( pathPlanner_ );
        skelGraphicsI_->localSetSkel( ogMap, &(skelPathPlanner->skeleton()) );
    }
    else
    {
        const bool displayDenseSkelFirst = false;
        if ( displayDenseSkelFirst )
        {
            orcapathplan::SkeletonPathPlanner *temp = 
                    new orcapathplan::SkeletonPathPlanner( ogMap,
                    robotDiameterMetres,
                    traversabilityThreshhold,
                    doPathOptimization );
            skelGraphicsI_->localSetSkel( ogMap, &(temp->skeleton()) );
        }
        
        
        orcapathplan::SparseSkeletonPathPlanner *sparseSkelPathPlanner = dynamic_cast<orcapathplan::SparseSkeletonPathPlanner*>( pathPlanner_ );
        skelGraphicsI_->localSetSkel( ogMap,
                                      &(sparseSkelPathPlanner->denseSkel()),
                                      &(sparseSkelPathPlanner->sparseSkel()) );
    }
}
#endif

void 
SkeletonDriver::computePath( const orca::PathPlanner2dTask& task,
                             orca::PathPlanner2dData& pathData )
{
    genericDriver_->computePath( task, pathData );
}

}
