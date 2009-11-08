#ifndef SEGWAYRMP_PUBLISHERTHREAD_H
#define SEGWAYRMP_PUBLISHERTHREAD_H

#include <orcaice/subsystemthread.h>
#include <hydrointerfaces/segwayrmp.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaifaceimpl/odometry2d.h>
#include <orcaifaceimpl/odometry3d.h>
#include <orcaifaceimpl/power.h>
#include <hydroutil/history.h>
#include "stats.h"

namespace segwayrmp {

//
// @brief Responsible for publishing data over orca interfaces.
//        We publish in a separate thread because:
//          a) it avoids network-delays interacting with the driver thread, and
//          b) it allows us to publish at a rate different from the at which rate data is generated.
//
// @author Alex Brooks
//
class PublisherThread : public orcaice::SubsystemThread
{

public:

    // intervals in seconds
    PublisherThread( double                                  odometry2dPublishInterval,
                     double                                  odometry3dPublishInterval,
                     double                                  powerPublishInterval,
                     const orcaifaceimpl::Odometry2dImplPtr &odometry2dI,
                     const orcaifaceimpl::Odometry3dImplPtr &odometry3dI,
                     const orcaifaceimpl::PowerImplPtr      &powerI,
                     gbxutilacfr::Tracer                    &tracer,
                     gbxutilacfr::Status                    &status,
                     hydroutil::History                     &history )
        : orcaice::SubsystemThread(tracer,status,"PublisherThread"),
          odometry2dPublishInterval_(odometry2dPublishInterval),
          odometry3dPublishInterval_(odometry3dPublishInterval),
          powerPublishInterval_(powerPublishInterval),
          odometry2dI_(odometry2dI),
          odometry3dI_(odometry3dI),
          powerI_(powerI),
          history_(history),
          hardwareInitNeedsReporting2d_(false),
          hardwareInitNeedsReporting3d_(false)
        {}

    // from SubsystemThread
    virtual void work();
    virtual void finalise();

    void publish( const hydrointerfaces::SegwayRmp::Data &data );

    void hardwareInitialised();

private:

    double                            odometry2dPublishInterval_;
    double                            odometry3dPublishInterval_;
    double                            powerPublishInterval_;

    orcaifaceimpl::Odometry2dImplPtr  odometry2dI_;
    orcaifaceimpl::Odometry3dImplPtr  odometry3dI_;
    orcaifaceimpl::PowerImplPtr       powerI_;

    gbxiceutilacfr::Store<hydrointerfaces::SegwayRmp::Data> dataStore_;

    // Summarizes motion information
    Stats historyStats_;
    hydroutil::History &history_;

    bool hardwareInitNeedsReporting2d_;
    bool hardwareInitNeedsReporting3d_;
};

}

#endif
