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
// @brief responsible for publishing data over orca interfaces
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
          history_(history)
        {}

    // from SubsystemThread
    virtual void work();
    virtual void finalise();

    void publish( const hydrointerfaces::SegwayRmp::Data &data )
        {
            stats_.addData( data );
            dataStore_.set( data ); 
        }
    
    void hardwareInitialised()
        { stats_.resetRawData(); }

private: 

    double                            odometry2dPublishInterval_;
    double                            odometry3dPublishInterval_;
    double                            powerPublishInterval_;
                     
    orcaifaceimpl::Odometry2dImplPtr  odometry2dI_;
    orcaifaceimpl::Odometry3dImplPtr  odometry3dI_;
    orcaifaceimpl::PowerImplPtr       powerI_;

    gbxiceutilacfr::Store<hydrointerfaces::SegwayRmp::Data> dataStore_;

    // Summarizes motion information
    Stats stats_;

    hydroutil::History &history_;
};

}

#endif
