/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_HARDWARE_HANDLER_H
#define ORCA2_SEGWAY_RMP_HARDWARE_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/notify.h>
#include <orcaice/proxy.h>
#include <orcaice/timer.h>

#include <orca/velocitycontrol2d.h>
#include <orca/vehicledescription.h>

#include "hwdriver.h"
#include "types.h"

namespace segwayrmp
{

class RmpIo;

//
// A thread that looks after the Segway hardware (ie HwDriver).
//
// Note: this thing self-destructs when run() returns.
class HwHandler : public orcaice::Thread, public orcaice::NotifyHandler<orca::VelocityControl2dData>
{
public:
        
    // The VehicleDescription is non-const, so the handler (and driver)
    // can update it with actual hardware limits.
    HwHandler( orcaice::Proxy<Data>& dataPipe,
               orcaice::Notify<orca::VelocityControl2dData>& commandPipe,
               orca::VehicleDescription&             descr,
               const orcaice::Context& context );
    virtual ~HwHandler();

    // from Thread
    virtual void run();

    // from PtrNotifyHandler
    virtual void handleData( const orca::VelocityControl2dData & obj );

private:

    // Keeps trying until success or !isActive()
    void enableDriver();

    // network/hardware interface
    orcaice::Proxy<Data>& dataPipe_;

    // generic interface to the hardware
    RmpIo*    rmpIo_;
    HwDriver* driver_;

    struct Config
    {
        bool   isMotionEnabled;
        double maxSpeed;
        double maxTurnrate;
    };
    Config config_;

    // component current context
    orcaice::Context context_;

    // This structs stores fault information.  
    class FaultInfo {
    public:
        FaultInfo( bool isError=false )
            : isError_(isError) {}
        FaultInfo( bool isError, std::string diagnostics )
            : isError_(isError),
              diagnostics_(diagnostics)
            {}

        bool isError() const { return isError_; }
        const std::string &diagnostics() { return diagnostics_; }
    private:
        bool isError_;
        std::string diagnostics_;
    };
    //
    // Faults can be detected in either read or write threads: have to be careful.
    //
    orcaice::Proxy<FaultInfo> faultProxy_;

    // debug
    orcaice::Timer readTimer_;
    orcaice::Timer writeTimer_;

    // utilities
    static void convert( const orca::VelocityControl2dData& network, segwayrmp::Command& internal );
};

} // namespace

#endif
