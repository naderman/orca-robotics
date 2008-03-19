/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_SKELETON_GRAPHICS_I
#define ORCA_SKELETON_GRAPHICS_I


// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/qgraphics2d.h>

#include <hydropathplan/hydropathplan.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>
#include <vector>
#include <qpainter.h>

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
    // Leave either skell NULL and it won't be drawn.
    void localSetSkel( const hydroogmap::OgMap                     &ogMap,
                       const hydropathplan::Cell2DVector           *skel=NULL,
                       const hydropathplan::sparseskel::SparseSkel *sparseSkel=NULL );

private:

    void drawSkel( const hydroogmap::OgMap           &ogMap,
                   const hydropathplan::Cell2DVector &skel,
                   QPainter                          &p );
    void drawSparseSkel( const hydroogmap::OgMap                     &ogMap,
                         const hydropathplan::sparseskel::SparseSkel &skel,
                         QPainter                                    &p );
    
    gbxsickacfr::gbxiceutilacfr::Store<orca::QGraphics2dData> dataStore_;

    orca::QGraphics2dConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx              topicPrx_;
    orcaice::Context                context_;
};

}
#endif
