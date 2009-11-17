#ifndef SEGWAYRMP_SEGWAYRMP_H
#define SEGWAYRMP_SEGWAYRMP_H

#include <hydrormputil/powerbasemanager.h>
#include <hydroutil/context.h>

namespace segwayrmp {

//
// @brief Simple API for the SegwayRMP.
//        For definitions of the sturctures in the hydrointerfaces namespace, see:
//        http://orca-robotics.sourceforge.net/hydro/classhydrointerfaces_1_1SegwayRmp.html
//
// @author Alex Brooks
//
class SegwayRmp
{
public: 

    //
    // capabilitiesConstraints: user-specified constraints on the vehicle 
    //                          (gets 'min-ed' with actual vehicle capabilities, useful for testing safely)
    // callback:                used to return odometric data
    // fakeDriver:              if true, we set up a fake Segway rather than talking to real hardware.
    //
    SegwayRmp( hydrointerfaces::SegwayRmp::Capabilities &capabilitiesConstraints,
               double                                    maxForwardAcceleration,
               double                                    maxReverseAcceleration,
               hydrormputil::AggregatorCallback          &callback,
               bool                                      fakeDriver,
               const hydroutil::Context                 &context );

    // Set the desired velocity setpoint.
    // Note that if a command is not set for 200ms, the velocity will be slewed to zero
    // (this is a safety mechanism to help prevent runaway robots).
    void setCommand( const hydrointerfaces::SegwayRmp::Command &incomingCmd );

    // Retreive the robot's capabilities
    const hydrointerfaces::SegwayRmp::Capabilities &capabilities() const { return capabilities_; }

private: 

    hydrointerfaces::SegwayRmp::Capabilities      capabilities_;
    std::vector<hydrormputil::PowerbaseManagerPtr> powerbaseManagers_;
    hydroutil::Context                            context_;
};

}

#endif
