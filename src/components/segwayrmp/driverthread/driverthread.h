#ifndef SEGWAYRMPDRIVERTHREAD_DRIVERTHREAD_H
#define SEGWAYRMPDRIVERTHREAD_DRIVERTHREAD_H

#include <orcaice/subsystemthread.h>
#include <hydrointerfaces/segwayrmp.h>
#include <orcarobotdriverutil/statemachine.h>
#include <driverthread/stallsensor.h>

namespace segwayrmpdriverthread {

class Callback {
public:

    virtual ~Callback() {}

    //! Called when communication to the hardware is established
    virtual void hardwareInitialised()=0;
    //! Called when data is read from the hardware
    virtual void receiveData( const hydrointerfaces::SegwayRmp::Data &data )=0;
};

//
// @author Alex Brooks
//
class DriverThread : public orcaice::SubsystemThread
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
        : orcaice::SubsystemThread(tracer,status,name),
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

    const orcarobotdriverutil::StateMachine &stateMachine()
        { return stateMachine_; }

private: 

    void enableHardware();
    void operateHardware();

    // Stores incoming commands that need to be written to the hardware
    gbxiceutilacfr::Store<hydrointerfaces::SegwayRmp::Command> commandStore_;

    orcarobotdriverutil::StateMachine stateMachine_;

    Callback &callback_;

    Config                      config_;
    hydrointerfaces::SegwayRmp &segwayRmp_;
    gbxutilacfr::Tracer        &tracer_;
};

}

#endif
