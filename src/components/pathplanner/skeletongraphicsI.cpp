/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "skeletongraphicsI.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <qpicture.h>

using namespace std;
using namespace orca;
using namespace orcaice;

namespace pathplanner {

SkeletonGraphicsI::SkeletonGraphicsI( Context context, const std::string &tag )
    : context_(context)
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<QGraphics2dConsumerPrx>
        ( context_, consumerPrx_, tag );
}

QGraphics2dDataPtr
SkeletonGraphicsI::getData(const Ice::Current& current) const
{
    // create a null pointer. data will be cloned into it.
    orca::QGraphics2dDataPtr data;

    // we don't need to pop the data here because we don't block on it.
    // we always want to have the latest copy in there
    if ( dataBuffer_.isEmpty() )
    {
        throw orca::DataNotExistException( "No graphics available." );
    }
    dataBuffer_.get( data );
    return data;
}

void
SkeletonGraphicsI::subscribe(const ::QGraphics2dConsumerPrx& subscriber,
                  const Ice::Current& current)
{
    cout<<"subscribe()"<<endl;
    IceStorm::QoS qos;
    topicPrx_->subscribe( qos, subscriber );
}

void
SkeletonGraphicsI::unsubscribe(const ::QGraphics2dConsumerPrx& subscriber,
                    const Ice::Current& current)
{
    cout<<"unsubscribe()"<<endl;
    topicPrx_->unsubscribe( subscriber );
}


void 
SkeletonGraphicsI::drawSkel( const orcaogmap::OgMap           &ogMap,
                             const orcapathplan::Cell2DVector &skel,
                             QPainter                         &p )
{
    // Some of these skels get pretty large.
    // Reduce the size of the dense skel we display...
    const int MAX_SKEL = 1000;
    int n = (int) ceil( skel.size() / MAX_SKEL );
    if ( n==0 ) n=1;
    orcapathplan::Cell2DVector denseSkelToDisplay;
    for ( unsigned int i=0; i < skel.size(); i+=n )
    {
        denseSkelToDisplay.push_back( skel[i] );
    }

    QColor color = Qt::darkCyan;
    color.setAlpha( 128 );

    p.setPen( color );
    p.setBrush( color );
    const double circleSize = 0.1;     // in m, should be constant pixel size?
        
    for (unsigned int i=0; i<denseSkelToDisplay.size(); i++ )
    {
        float worldX, worldY;
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
SkeletonGraphicsI::drawSparseSkel( const orcaogmap::OgMap           &ogMap,
                                   const orcapathplan::SparseSkel   &skel,
                                   QPainter                         &p )
{
    bool print = false;

    if ( print )
        cout<<"TRACE(skeletongraphicsI.cpp): Skeleton details:" << endl;

    QColor color = Qt::red;
    color.setAlpha( 128 );

    p.setPen( color );
    p.setBrush( color );
    const double circleSize = 0.3;     // in m, should be constant pixel size?
    
    for ( unsigned int i=0; i < skel.contiguousSkels().size(); i++ )
    {
        const std::vector<orcapathplan::SparseSkelNode*> &nodes = skel.contiguousSkels()[i]->nodes();
        for ( unsigned int j=0; j < nodes.size(); j++ )
        {
            const orcapathplan::SparseSkelNode *node = nodes[j];
            float nodeX, nodeY;

            // draw node
            ogMap.getWorldCoords( node->pos.x(), node->pos.y(), nodeX, nodeY );
            p.drawEllipse( QRectF( nodeX-circleSize, nodeY-circleSize, 2*circleSize, 2*circleSize ) );

            if ( print )
                cout<<"  Node at " << nodeX << "," << nodeY << ":" << endl;
                

            // draw arcs
            for ( unsigned int k=0; k < node->arcs.size(); k++ )
            {
                const orcapathplan::SparseSkelArc *arc = node->arcs[k];
                float toX, toY;
                ogMap.getWorldCoords( arc->toNode->pos.x(), arc->toNode->pos.y(), toX, toY );
                p.drawLine( QLineF( nodeX, nodeY, toX, toY ) );

                if ( print )
                    cout << "    links to " << toX << "," << toY << " with cost " << arc->cost*ogMap.metresPerCellX() << endl;
            }
        }
    }
}

void 
SkeletonGraphicsI::localSetSkel( const orcaogmap::OgMap           &ogMap,
                                 const orcapathplan::Cell2DVector *skel,
                                 const orcapathplan::SparseSkel   *sparseSkel )
{
    //
    // Set up the orca object
    //

    // First draw in a QPicture
    QPicture qpic;
    QPainter p;
    p.begin( &qpic );
    {
        if ( skel != NULL )
            drawSkel( ogMap, *skel, p );
        if ( sparseSkel != NULL )
            drawSparseSkel( ogMap, *sparseSkel, p );
    }
    p.end();

    // Then stick it in the orca object
    orca::QGraphics2dDataPtr data = new QGraphics2dData;

    data->z = 4;
    data->isInGlobalCS = true;

    data->qpicture.resize( qpic.size() );
    memcpy( &(data->qpicture[0]), qpic.data(), qpic.size() );

    // Stick it in the buffer
    dataBuffer_.push( data );
    
    // Then push to IceStorm.
    try {
        consumerPrx_->setData( data );
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
