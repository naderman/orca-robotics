/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>

#include <vector>

namespace pathplanner
{

class Point2d
{
public:
    Point2d( float x, float y ) { x_=x; y_=y; };
    ~Point2d() {};
    float x() const { return x_; };
    float y() const { return y_; };
    void setX( float x ) { x_ = x; };
    void setY( float y ) { y_ = y; };

private:
    float x_;
    float y_;
};

typedef std::vector<Point2d> Point2dVector;

class SkeletonGraphicsI : public orca::QGraphics2d
{
public:

    SkeletonGraphicsI( orcaice::Context context, const std::string &tag );

    // Remote calls:

    // Hand out data to people
    orca::QGraphics2dDataPtr getData(const Ice::Current&) const;

    virtual void subscribe(const ::orca::QGraphics2dConsumerPrx&,
                           const Ice::Current&);

    virtual void unsubscribe(const ::orca::QGraphics2dConsumerPrx&,
                             const Ice::Current&);

    // Local calls:
    void localSetData( const Point2dVector & skeletonWorld );

private:

    orcaice::PtrBuffer<orca::QGraphics2dDataPtr> dataBuffer_;

    orca::QGraphics2dConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx              topicPrx_;
    orcaice::Context                context_;
};

}
#endif
