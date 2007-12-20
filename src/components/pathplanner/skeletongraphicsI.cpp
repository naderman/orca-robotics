/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "skeletongraphicsI.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <qpicture.h>
#include <hydropathplan/sparseskel/sparseskel.h>

using namespace std;
using namespace orca;
using namespace orcaice;

namespace pathplanner {

SkeletonGraphicsI::SkeletonGraphicsI( const orcaice::Context & context, const std::string &tag )
    : context_(context)
{    
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<QGraphics2dConsumerPrx>
        ( context_, consumerPrx_, tag );
}

QGraphics2dData
SkeletonGraphicsI::getData(const Ice::Current& current) const
{
    // create a null pointer. data will be cloned into it.
    orca::QGraphics2dData data;

    // we don't need to pop the data here because we don't block on it.
    // we always want to have the latest copy in there
    if ( dataStore_.isEmpty() )
    {
        throw orca::DataNotExistException( "No graphics available." );
    }
    dataStore_.get( data );
    return data;
}

void
SkeletonGraphicsI::subscribe(const ::QGraphics2dConsumerPrx& subscriber,
                             const Ice::Current& current)
{
    cout<<"subscribe()"<<endl;
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer().debug( ss.str(), 2 );    
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"subscribe: failed to subscribe: "<< e << endl;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void
SkeletonGraphicsI::unsubscribe(const ::QGraphics2dConsumerPrx& subscriber,
                    const Ice::Current& current)
{
    cout<<"unsubscribe()"<<endl;
    topicPrx_->unsubscribe( subscriber );
}


void 
SkeletonGraphicsI::drawSkel( const hydroogmap::OgMap           &ogMap,
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
        double worldX, worldY;
        ogMap.getWorldCoords( denseSkelToDisplay[i].x(),
                              denseSkelToDisplay[i].y(),
                              worldX,
                              worldY );
        p.drawEllipse( QRectF( worldX-circleSize, worldY-circleSize, 2*circleSize, 2*circleSize ) );
    }

//======== This should work but doesn't for some reason ==================
//         QPointF qpointArray[denseSkelToDisplay.size()];
//         for (unsigned int i=0; i<denseSkelToDisplay.size(); i++ )
//         {
//             qpointArray[i] = QPointF( skeletonWorld[i].x(), skeletonWorld[i].y() );
//         }
//         p.drawPolyline( &qpointArray[0], size );
}

void 
SkeletonGraphicsI::drawSparseSkel( const hydroogmap::OgMap                     &ogMap,
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
            double nodeX, nodeY;

            // draw node
            ogMap.getWorldCoords( node->pos.x(), node->pos.y(), nodeX, nodeY );
            p.drawEllipse( QRectF( nodeX-circleSize, nodeY-circleSize, 2*circleSize, 2*circleSize ) );

            if ( print )
                cout<<"  Node at " << nodeX << "," << nodeY << ":" << endl;
                

            // draw arcs
            for ( unsigned int k=0; k < node->arcs.size(); k++ )
            {
                const hydropathplan::sparseskel::SparseSkelArc *arc = node->arcs[k];
                double toX, toY;
                ogMap.getWorldCoords( arc->toNode->pos.x(), arc->toNode->pos.y(), toX, toY );
                p.drawLine( QLineF( nodeX, nodeY, toX, toY ) );

                if ( print )
                    cout << "    links to " << toX << "," << toY << " with cost " << arc->cost*ogMap.metresPerCellX() << endl;
            }
        }
    }
}

void 
SkeletonGraphicsI::localSetSkel( const hydroogmap::OgMap                     &ogMap,
                                 const hydropathplan::Cell2DVector           *skel,
                                 const hydropathplan::sparseskel::SparseSkel *sparseSkel )
{
    //
    // Set up the orca object
    //

    // First draw in a QPicture
    QPicture qpic;
    QPainter p;
    p.begin( &qpic );
    {
        if ( sparseSkel != NULL )
            drawSparseSkel( ogMap, *sparseSkel, p );
        if ( skel != NULL )
            drawSkel( ogMap, *skel, p );
    }
    p.end();

    // Then stick it in the orca object
    orca::QGraphics2dData data;

    data.z = 4;
    data.isInGlobalCS = true;

    data.qpicture.resize( qpic.size() );
    memcpy( &(data.qpicture[0]), qpic.data(), qpic.size() );

    // Stick it in the buffer
    dataStore_.set( data );
    
    // Then push to IceStorm.
    try {
        consumerPrx_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down, 
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer().warning( "Failed push to IceStorm." );
    }
    catch ( Ice::Exception &e )
    {
        cout << "ERROR(skeletongraphicsI.cpp): Caught exception while setting QGraphics object: " << e << endl;
        throw;
    }
}

}
