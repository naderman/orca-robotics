#include "driver.h"
#include <iostream>
#include <hydropathplan/hydropathplan.h>
#include <sstream>

#ifdef QT4_FOUND
#include <QPainter>
#include <QPicture>
#include <hydropathplan/sparseskel/sparseskel.h>
#endif

using namespace std;

namespace pathplanner2dacfr {

namespace {

    //
    // Cost function for planner
    //
    class DistBasedCostEvaluator : public hydropathplan::CostEvaluator {
    public:
        DistBasedCostEvaluator( double distanceThreshold,
                                double costMultiplier )
            : distanceThreshold_(distanceThreshold),
              costMultiplier_(costMultiplier)
            {}

        double costAtDistFromObstacle( double distInMetres ) const
            {
                if ( distInMetres < 0 )
                    return NAN;
                else if ( distInMetres < distanceThreshold_ ) 
                    return costMultiplier_; 
                else 
                    return 1;
            }
        
    private:
        double distanceThreshold_;
        double costMultiplier_;
    };

#ifdef QT4_FOUND
    void 
    drawSparseSkel( const hydroogmap::OgMap                     &ogMap,
                    const hydropathplan::sparseskel::SparseSkel &skel,
                    QPainter                                    &p )
    {
        bool print = false;

        if ( print )
            cout<<"TRACE(skeletongraphicsI.cpp): Skeleton details:" << endl;

        std::vector<QColor> colors;
        colors.push_back(Qt::red);
        colors.push_back(Qt::blue);
        colors.push_back(Qt::cyan);
        colors.push_back(Qt::magenta);
        colors.push_back(Qt::darkYellow);    
        colors.push_back(Qt::darkRed);
        colors.push_back(Qt::darkBlue);
        colors.push_back(Qt::darkGreen);
        colors.push_back(Qt::darkCyan);
        colors.push_back(Qt::darkMagenta);

        // color.setAlpha( 128 );

        const double circleSize = 0.2;     // in m, should be constant pixel size?
    
        for ( unsigned int i=0; i < skel.contiguousSkels().size(); i++ )
        {
            QColor color = colors[i % colors.size()];
            p.setPen( color );
            p.setBrush( color );
    
            const std::vector<hydropathplan::sparseskel::SparseSkelNode*> &nodes = skel.contiguousSkels()[i]->nodes();
            for ( unsigned int j=0; j < nodes.size(); j++ )
            {
                const hydropathplan::sparseskel::SparseSkelNode *node = nodes[j];

                // draw node
                hydroogmap::WorldCoords nodeCoords(ogMap.worldCoords( node->pos.x(), node->pos.y()) );
                p.drawEllipse( QRectF( nodeCoords.x-circleSize, nodeCoords.y-circleSize, 2*circleSize, 2*circleSize ) );

                if ( print )
                    cout<<"  Node at " << nodeCoords << ":" << endl;
                

                // draw arcs
                for ( unsigned int k=0; k < node->arcs.size(); k++ )
                {
                    const hydropathplan::sparseskel::SparseSkelArc *arc = node->arcs[k];
                    hydroogmap::WorldCoords to( ogMap.worldCoords( arc->toNode->pos.x(), arc->toNode->pos.y() ) );
                    p.drawLine( QLineF( nodeCoords.x, nodeCoords.y, to.x, to.y ) );

                    if ( print )
                        cout << "    links to " << to << " with cost " << arc->cost*ogMap.metresPerCell() << endl;
                }
            }
        }
    }

    void drawSkel( const hydroogmap::OgMap           &ogMap,
                   const hydropathplan::Cell2DVector &skel,
                   QPainter                         &p )
    {
        // Some of these skels get pretty large.
        // Reduce the size of the dense skel we display...
        const int MAX_SKEL = 1000;
        int n = (int) ceil( skel.size() / MAX_SKEL );
        if ( n==0 ) n=1;
        hydropathplan::Cell2DVector denseSkelToDisplay;
        for ( unsigned int i=0; i < skel.size(); i+=n )
        {
            denseSkelToDisplay.push_back( skel[i] );
        }

        QColor color = Qt::darkCyan;
        // color.setAlpha( 128 );

        p.setPen( color );
        p.setBrush( color );
        const double circleSize = 0.1;     // in m, should be constant pixel size?
        
        for (unsigned int i=0; i<denseSkelToDisplay.size(); i++ )
        {
            hydroogmap::WorldCoords worldCoords( ogMap.worldCoords( denseSkelToDisplay[i].x(),
                                                                    denseSkelToDisplay[i].y() ) );
            p.drawEllipse( QRectF( worldCoords.x-circleSize, worldCoords.y-circleSize, 2*circleSize, 2*circleSize ) );
        }

        //======== This should work but doesn't for some reason ==================
        //         QPointF qpointArray[denseSkelToDisplay.size()];
        //         for (unsigned int i=0; i<denseSkelToDisplay.size(); i++ )
        //         {
        //             qpointArray[i] = QPointF( skeletonWorld[i].x(), skeletonWorld[i].y() );
        //         }
        //         p.drawPolyline( &qpointArray[0], size );
    }

    QPicture
    getSkeletonPicture( const hydroogmap::OgMap                              &ogMap,
                                 const hydropathplan::Cell2DVector           *skel=NULL,
                                 const hydropathplan::sparseskel::SparseSkel *sparseSkel=NULL )
    {
        QPicture pic;
        QPainter p;
        p.begin( &pic );
        {
            if ( sparseSkel != NULL )
                drawSparseSkel( ogMap, *sparseSkel, p );
            if ( skel != NULL )
                drawSkel( ogMap, *skel, p );
        }
        p.end();
        return pic;
    }

    void
    publishSkeletonGraphics( hydropathplan::IPathPlanner2d                        &pathPlanner,
                             hydrointerfaces::PathPlanner2d::QGraphics2dPublisher &graphicsPublisher,
                             bool                                                  useSparseSkeleton,
                             const hydroogmap::OgMap                              &ogMap,
                             double                                                robotDiameterMetres,
                             double                                                traversabilityThreshhold,
                             bool                                                  doPathOptimization )
    {                   
        if ( !useSparseSkeleton )
        {
            hydropathplan::SkeletonPathPlanner *skelPathPlanner = 
                dynamic_cast<hydropathplan::SkeletonPathPlanner*>( &pathPlanner );
            assert( skelPathPlanner != NULL );
            QPicture pic = getSkeletonPicture( ogMap, &(skelPathPlanner->skeleton()) );
            graphicsPublisher.localSetAndSend( pic );
        }
        else
        {
            const bool displayDenseSkelFirst = false;
            if ( displayDenseSkelFirst )
            {
                hydropathplan::SkeletonPathPlanner *temp = 
                    new hydropathplan::SkeletonPathPlanner( ogMap,
                                                            robotDiameterMetres,
                                                            traversabilityThreshhold,
                                                            doPathOptimization );
                QPicture pic = getSkeletonPicture( ogMap, &(temp->skeleton()) );
            graphicsPublisher.localSetAndSend( pic );
            }        
        
            hydropathplan::SparseSkeletonPathPlanner *sparseSkelPathPlanner = 
                dynamic_cast<hydropathplan::SparseSkeletonPathPlanner*>( &pathPlanner );
            assert( sparseSkelPathPlanner != NULL );
            QPicture pic = getSkeletonPicture( ogMap, 
                                               &(sparseSkelPathPlanner->denseSkel()),
                                               &(sparseSkelPathPlanner->sparseSkel()) );
            graphicsPublisher.localSetAndSend( pic );
        }
    }
#endif

}

Driver::Driver( const hydrointerfaces::PathPlanner2d::Config &config,
                const hydroutil::Context                     &context )
    : ogMap_(config.ogMap)
{
    std::string prefix = "Acfr.";
    const hydroutil::Properties &prop = context.properties();
    const hydroogmap::OgMap &ogMap = config.ogMap;


    //
    // Generic settings
    //
    const bool   doPathOptimization = prop.getPropertyAsIntWithDefault( prefix+"DoPathOptimization", 0 );
    const double traversabilityThreshhold = prop.getPropertyAsDoubleWithDefault( prefix+"TraversabilityThreshhold", 0.3 );
    const double robotDiameterMetres = prop.getPropertyAsDoubleWithDefault( prefix+"RobotDiameterMetres", 0.8 );

    // based on the config parameter, create the right driver
    string algorithm = prop.getPropertyWithDefault( prefix+"Algorithm", "simplenav" );
    context.tracer().debug( std::string("loading ")+algorithm+" algorithm",3);
    
    if ( algorithm == "simplenav" )
    {
        algorithm_.reset( new hydropathplan::SimpleNavPathPlanner( ogMap,
                                                                   robotDiameterMetres,
                                                                   traversabilityThreshhold,
                                                                   doPathOptimization ) );
    } 
    else if (algorithm == "astar") 
    {
        algorithm_.reset( new hydropathplan::AStarPathPlanner( ogMap,
                                                               robotDiameterMetres,
                                                               traversabilityThreshhold,
                                                               doPathOptimization ) );
    }
    else if ( algorithm == "skeletonnav" || algorithm == "sparseskeletonnav" )
    {
        double distanceThreshold = prop.getPropertyAsDoubleWithDefault( prefix+"Skeleton.Cost.DistanceThreshold", 0.3 );
        double costMultiplier = prop.getPropertyAsDoubleWithDefault( prefix+"Skeleton.Cost.CostMultiplier", 10 );

        DistBasedCostEvaluator costEvaluator( distanceThreshold, costMultiplier );

        bool useSparseSkeleton = (algorithm == "sparseskeletonnav");
        if ( useSparseSkeleton )
        {
            double addExtraSparseSkelNodes = prop.getPropertyAsDoubleWithDefault( prefix+"Skeleton.SparseSkelAddExtraNodes", 1 );
            double sparseSkelExtraNodeResolution = prop.getPropertyAsDoubleWithDefault( prefix+"Skeleton.SparseSkelExtraNodeResolution", 5 );
            bool jiggleWaypointsOntoClearCells = 
                prop.getPropertyAsIntWithDefault( prefix+"JiggleWaypointsOntoClearCells", true );

            algorithm_.reset( new hydropathplan::SparseSkeletonPathPlanner( ogMap,
                                                                            robotDiameterMetres,
                                                                            traversabilityThreshhold,
                                                                            doPathOptimization,
                                                                            addExtraSparseSkelNodes,
                                                                            sparseSkelExtraNodeResolution,
                                                                            jiggleWaypointsOntoClearCells,
                                                                            costEvaluator ) );
        }
        else
        {
            algorithm_.reset( new hydropathplan::SkeletonPathPlanner( ogMap,
                                                                      robotDiameterMetres,
                                                                      traversabilityThreshhold,
                                                                      doPathOptimization,
                                                                      costEvaluator ) );
        }
#ifdef QT4_FOUND
        if ( config.graphicsPublisher )
        {
            config.graphicsPublisher->init();

            bool provideSkeletonGraphics = prop.getPropertyAsDoubleWithDefault( prefix+"Skeleton.ProvideGraphics", 1 );
            if ( provideSkeletonGraphics && ( config.graphicsPublisher!=NULL ) )
            {
                publishSkeletonGraphics( *algorithm_,
                                         *config.graphicsPublisher,
                                         useSparseSkeleton,
                                         ogMap,
                                         robotDiameterMetres,
                                         traversabilityThreshhold,
                                         doPathOptimization );
            }
        }
#endif
    }
    else
    {
        // Only get to here if we didn't return a pathPlanner earlier.
        stringstream ssUnknown;
        ssUnknown << "Unknown algorithm: " << algorithm;
        throw gbxutilacfr::Exception( ERROR_INFO, ssUnknown.str() );    
    }
}

std::vector<hydrointerfaces::PathPlanner2d::Point>
Driver::computePath( const Point &start,
                     const Point &goal )
{
    //
    // Convert to discrete coordinates
    //
    hydroogmap::GridIndices startCell = ogMap_.gridIndices( start.x, start.y );
    hydroogmap::GridIndices goalCell  = ogMap_.gridIndices( goal.x, goal.y );


    //
    // Compute the path 
    //
    hydropathplan::Cell2DVector cellVector;
    try
    {
        algorithm_->computePath( startCell.x, startCell.y,
                                 goalCell.x, goalCell.y,
                                 cellVector );
    }
    catch ( hydropathplan::PathDestinationUnreachableException &e )
    {
        throw hydrointerfaces::PathPlanner2d::PathDestinationUnreachableException( ERROR_INFO, e.what() );
    }
    catch ( hydropathplan::PathDestinationNotValidException &e )
    {
        throw hydrointerfaces::PathPlanner2d::PathDestinationNotValidException( ERROR_INFO, e.what() );
    }
    catch ( hydropathplan::PathStartNotValidException &e )
    {
        throw hydrointerfaces::PathPlanner2d::PathStartNotValidException( ERROR_INFO, e.what() );
    }
    catch ( hydropathplan::Exception &e )
    {
        throw hydrointerfaces::PathPlanner2d::Exception( ERROR_INFO, e.what() );
    }

    //
    // Convert to continuous coordinates
    //
    std::vector<hydrointerfaces::PathPlanner2d::Point> path(cellVector.size());
    for ( size_t i=0; i < cellVector.size(); i++ )
    {
        ogMap_.getWorldCoords( cellVector[i].x(), cellVector[i].y(),
                               path[i].x, path[i].y );
    }
    return path;
}

} // namespace

extern "C" {
    hydrointerfaces::PathPlanner2dFactory *createDriverFactory()
    { return new pathplanner2dacfr::Factory; }
}
