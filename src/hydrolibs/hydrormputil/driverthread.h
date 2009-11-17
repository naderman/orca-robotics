#ifndef HYDRORMPUTIL_DRIVERTHREAD_H
#define HYDRORMPUTIL_DRIVERTHREAD_H

#include <hydroiceutil/subsystemthread.h>
#include <hydrointerfaces/segwayrmp.h>
#include <hydrorobotdriverutil/statemachine.h>
#include <hydrormputil/stallsensor.h>

namespace hydrormputil {

class Callback {
public:

    virtual ~Callback() {}

    //! Called when communication to the hardware is established
    virtual void hardwareInitialised()=0;
    //! Called when data is read from the hardware
    virtual void receiveData( const hydrointerfaces::SegwayRmp::Data &data,
                              StallType                               stallType ) = 0;
};

//
// Note that this class derives from a SubsystemThread defined in libHydroIceUtil,
// not the one from libOrcaIce. See documentation for differences.
//
// @author Alex Brooks
//
class DriverThread : public hydroiceutil::SubsystemThread
{
public:

    ////////////////////////////////////////

    struct Config {
        // If set, the driverthread reverses the controls.
        bool   driveInReverse;
        // Allows testing without fear of it taking off
        bool   isMotionEnabled;

        // These acceleration limits are enforced by the driverthread.
        double maxForwardAcceleration;
        double maxReverseAcceleration;

        // For detection of stalls
        StallSensor::Config stallSensorConfig;
    };

    ////////////////////////////////////////

    DriverThread( const Config               &config,
                  hydrointerfaces::SegwayRmp &segwayRmp,
                  gbxutilacfr::Tracer        &tracer,
                  gbxutilacfr::Status        &status,
                  Callback                   &callback,
                  const std::string &name = "segwayRmpDriverThread" )
        : hydroiceutil::SubsystemThread(tracer,status,name),
          callback_(callback),
          config_(config),
          segwayRmp_(segwayRmp),
          tracer_(tracer)
        {}

    // from SubsystemThread
    virtual void initialise() {}
    virtual void work();
    virtual void finalise() {}

    void setDesiredSpeed( const hydrointerfaces::SegwayRmp::Command &command );

    const hydrorobotdriverutil::StateMachine &stateMachine()
        { return stateMachine_; }

private:

    void enableHardware();
    void operateHardware();

    // Stores incoming commands that need to be written to the hardware
    gbxiceutilacfr::Store<hydrointerfaces::SegwayRmp::Command> commandStore_;

    hydrorobotdriverutil::StateMachine stateMachine_;

    Callback &callback_;

    Config                      config_;
    hydrointerfaces::SegwayRmp &segwayRmp_;
    gbxutilacfr::Tracer        &tracer_;
};

}

#endif
