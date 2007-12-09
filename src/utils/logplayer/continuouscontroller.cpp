#include "continuouscontroller.h"
#include <iostream>

using namespace std;

namespace logplayer {

ContinuousController::ContinuousController( ReplayConductor        &replayConductor,
                                            bool                    autoStart,
                                            const orcaice::Context &context )
    : SafeThread( context.tracer(), context.status() ),
      replayConductor_(replayConductor),
      autoStart_(autoStart),
      context_(context)
{
}

void
ContinuousController::walk()
{
    if ( !autoStart_ )
    {
        cout<<"Type ENTER to start replay:" << endl;
        cout<<"==>"<<endl;
        getchar();
    }
    replayConductor_.startPlaying();

    // Wait for replayConductor to finish playing
    while ( !isStopping() &&
            replayConductor_.isPlayingOrAboutToStart() )
    {
        // cout<<"TRACE(continuouscontroller.cpp): isPlayingOrAboutToStart(): " << replayConductor_.isPlayingOrAboutToStart() << endl;
        sleep(1);
    }

    // Finished playing -- shut down.
    cout<<"TRACE(continuouscontroller.cpp): Finished playing -- shutting down." << endl;
    context_.communicator()->destroy();
}

}

