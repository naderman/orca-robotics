#ifndef ORCAROBOTDRIVERUTIL_ESTOPMONITOR_H
#define ORCAROBOTDRIVERUTIL_ESTOPMONITOR_H

#include <IceUtil/Mutex.h>
#include <orca/estop.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

namespace orcarobotdriverutil {

//
// @brief thread-safe class that maintains the state of the e-stop
//
// @author Alex Brooks
//
class EStopMonitor
{

public: 

    EStopMonitor( const orca::EStopDescription &description );

    // The reason is only set if the e-stop is triggered.
    bool isEStopTriggered( std::string &reason ) const;
    void addData( const orca::EStopData &data );

private: 

    mutable IceUtil::Mutex mutex_;

    const orca::EStopDescription description_;

    orca::EStopData state_;

    gbxiceutilacfr::Timer keepAliveTimer_;
};

}

#endif
