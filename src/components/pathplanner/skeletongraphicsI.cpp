#include "skeletongraphicsI.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <qpainter.h>
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
        cout << "ERROR(skeletongraphicsI.cpp): Buffer was empty!" << endl;
        exit(1);
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
SkeletonGraphicsI::localSetData( const Point2dVector & skeletonWorld )
{
    //
    // Set up the orca object
    //

    // First draw in a QPicture
    QColor color = Qt::darkCyan;
    color.setAlpha( 128 );

    QPicture qpic;
    QPainter p;
    p.begin( &qpic );
    {
        // Painting code
        p.setPen( color );
        p.setBrush( color );
        const double circleSize = 0.2;     // in m, should be constant pixel size?
        
        unsigned int size = skeletonWorld.size();       

        for (unsigned int i=0; i<size; i++ )
        {
            p.drawEllipse(QRectF( skeletonWorld[i].x()-circleSize, skeletonWorld[i].y()-circleSize, 2*circleSize, 2*circleSize) );
        }

//======== This should work but doesn't for some reason ==================
//         QPointF qpointArray[size];
//         for (unsigned int i=0; i<size; i++ )
//         {
//             qpointArray[i] = QPointF( skeletonWorld[i].x(), skeletonWorld[i].y() );
//         }
//         p.drawPolyline( &qpointArray[0], size );
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
