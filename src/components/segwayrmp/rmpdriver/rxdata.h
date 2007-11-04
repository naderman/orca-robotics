#ifndef SEGWAYRMP_RXDATA_H
#define SEGWAYRMP_RXDATA_H

#include <rmpdriver/rawrxdata.h>
#include <rmpdriver/unitconverter.h>

namespace segwayrmp {

class CanPacket;

//
// @brief Holds data received by the host from the RMP.
//        The actual data is stored in a RmpRawData structure.
//        Public member functions return the data in sane units.
//
// This thing is supposed to have a lifetime longer than an individual CAN packet.
// Since different CAN packets hold different kinds of data, this thing acts
// as an accumulator.
//
class RxData
{

public: 

    // Data is empty on construction.
    RxData( RmpModel rmpModel,
            const orcaice::Context &context );

    // Adds the information contained in a single CAN packet
    void addPacket( const CanPacket &pkt ) { rawData_.addPacket(pkt); }
    // Did all the packets arrive?
    bool isComplete() const { return rawData_.isComplete(); }

    //
    // Functions for retreiving the data in sane units.
    // Should only be called when the data structure is full.
    //
    // All units are S.I.
    //

    // Get all status flags
    std::vector<StatusFlag> statusFlags() const;
    // Is there a warning condition?
    bool isWarn() const;
    // Is there an error condition?
    bool isFault() const;

    // Not sure how to interpret the build ID, so just report it as-is.
    int16_t buildId() const
        {
            return rawData_.build_id; 
        }

    // roll/pitch/yaw
    double pitchAngle() const 
        {
            return converter_.angleInRadians(rawData_.pitch);
        }
    double pitchRate() const 
        {
            return converter_.angularRateInRadians(rawData_.pitch_dot);
        }
    double rollAngle() const
        {
            return converter_.angleInRadians(rawData_.roll);
        }
    double rollRate() const
        {
            return converter_.angularRateInRadians(rawData_.roll_dot);
        }
    double yaw() const
        {
            // Note special case: angle specified in revolutions
            return converter_.angleInRadiansFromRevCounts(rawData_.yaw);
        }
    double yawRate() const
        {
            // take the additive inverse, since the RMP reports
            // clockwise angular velocity as positive.
            return -converter_.angularRateInRadians(rawData_.yaw_dot);
        }
    
    // wheels
    double leftWheelDisplacement() const
        {
            return converter_.distanceInM(rawData_.left_wheel_displacement);
        }

    double rightWheelDisplacement() const
        {
            return converter_.distanceInM(rawData_.right_wheel_displacement);
        }
    double leftWheelVelocity() const
        {
            return converter_.speedInMperS(rawData_.left_wheel_velocity);
        }
    double rightWheelVelocity() const
        {
            return converter_.speedInMperS(rawData_.left_wheel_velocity);
        }

    // values based on wheels
    double foreAftDisplacement() const
        {
            return converter_.distanceInM(rawData_.foreaft_displacement);
        }
    double leftMotorTorque() const
        {
            return converter_.torqueInNM(rawData_.left_torque);
        }
    double rightMotorTorque() const
        {
            return converter_.torqueInNM(rawData_.right_torque);
        }

    // other gear
    int servoFrames()
        {
            return rawData_.servo_frames;
        }
    double uiBatteryVoltage()
        {
            return converter_.uiVoltageInVolts(rawData_.ui_battery_voltage);
        }
    // We only know the minimum of the two main batteries.
    double baseBatteryVoltage()
        {
            return converter_.baseVoltageInVolts(rawData_.base_battery_voltage);
        }
    OperationalMode operationalMode()
        {
            return converter_.operationalModeAsEnum(rawData_.operational_mode);
        }
    GainSchedule gainSchedule()
        {
            return converter_.gainScheduleAsEnum(rawData_.controller_gain_schedule);
        }

//     double receivedVelocityCommand()
//         {
//             return converter_.
//         }


    // Prints out in human-readable form
    std::string toString() const { return rawData_.toString(); }

    // Allow direct access to the raw data
    const RawRxData rawData() const { return rawData_; }

private: 

    // Stores the data in the RMP's native representation
    RawRxData rawData_;

    // Convert between units
    UnitConverter converter_;

};

}

#endif
