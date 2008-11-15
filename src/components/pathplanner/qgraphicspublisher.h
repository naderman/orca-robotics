#ifndef PATHPLANNER_QGRAPHICSPUBLISHER_H
#define PATHPLANNER_QGRAPHICSPUBLISHER_H

#ifdef QT4_FOUND
#include <hydrointerfaces/pathplanner2d.h>
#include <orcaifaceimpl/qgraphics2dImpl.h>
#include <QPicture>

namespace pathplanner {

//
// @brief Allows path-planning algorithms to visualise their internal details
//
// @author Alex Brooks
//
class QGraphicsPublisher : public hydrointerfaces::PathPlanner2d::QGraphics2dPublisher
{

public: 

    QGraphicsPublisher( const orcaice::Context &context,
                        const std::string &name );

    void init();
    void localSetAndSend( const QPicture &pic );

private: 

    orcaifaceimpl::QGraphics2dImplPtr qGraphics2dImpl_;
    orca::QGraphics2dData orcaPic_;
};

}
#endif
#endif
