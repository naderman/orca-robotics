/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_SKELETON_GRAPHICS_I
#define ORCA2_SKELETON_GRAPHICS_I


// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/qgraphics2d.h>

#include <orcapathplan/cell2d.h>
#include <orcapathplan/sparseskel.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>
#include <qpainter.h>
#include <vector>

namespace pathplanner
{

class SkeletonGraphicsI : public orca::QGraphics2d
{
public:

    SkeletonGraphicsI( const orcaice::Context & context, const std::string &tag );

    // Remote calls:

    // Hand out data to people
    orca::QGraphics2dData getData(const Ice::Current&) const;

    virtual void subscribe(const ::orca::QGraphics2dConsumerPrx&,
                           const Ice::Current&);

    virtual void unsubscribe(const ::orca::QGraphics2dConsumerPrx&,
                             const Ice::Current&);

    // Local calls:
    // Leave eith skell NULL and it won't be drawn.
    void localSetSkel( const orcaogmap::OgMap           &ogMap,
                       const orcapathplan::Cell2DVector *skel=NULL,
                       const orcapathplan::SparseSkel   *sparseSkel=NULL );

private:

    void drawSkel( const orcaogmap::OgMap           &ogMap,
                   const orcapathplan::Cell2DVector &skel,
                   QPainter                         &p );
    void drawSparseSkel( const orcaogmap::OgMap           &ogMap,
                         const orcapathplan::SparseSkel   &skel,
                         QPainter                         &p );
    
    orcaice::Buffer<orca::QGraphics2dData> dataBuffer_;

    orca::QGraphics2dConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx              topicPrx_;
    orcaice::Context                context_;
};

}
#endif
