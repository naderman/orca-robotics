#ifndef SEGWAYRMP_RMPRAWDATA_H
#define SEGWAYRMP_RMPRAWDATA_H

#include <orcaice/context.h>
#include <rmpdriver/canpacket.h>
#include <string>

namespace segwayrmp {

//
// @brief Holds data received by the host from the RMP, in the RMP's
// raw format (ie everything as counts).
//
// This thing is supposed to have a lifetime longer than an individual CAN packet.
// Since different CAN packets hold different kinds of data, this thing acts
// as an accumulator.
//
// The RmpRawData structure is complete when it has a status message
// and all 8 monitoring messages
//
class RmpRawData
{

public: 

    // Data is empty on construction.
    RmpRawData( const orcaice::Context &context );

    // Adds information from a packet
    void addPacket(const CanPacket &pkt);
    
    // Print out frame data to string
    std::string toString() const;

    // Did all the packets arrive?
    bool isComplete() const;

    // Build ID
    int16_t    build_id;
    // Pitch Angle
    int16_t    pitch;
    // Pitch Rate
    int16_t    pitch_dot;
    // Roll Angle
    int16_t    roll;
    // Roll Rate
    int16_t    roll_dot;
    // yaw displacement
    uint32_t   yaw_displacement;
    // Yaw rate
    int16_t    yaw_dot;
    // integrated left wheel displacement
    uint32_t   left_wheel_displacement;
    // Left wheel Velocity
    int16_t    left_wheel_velocity;
    // integrated right wheel displacement
    uint32_t   right_wheel_displacement;
    // Right wheel Velocity
    int16_t    right_wheel_velocity;
    // integrated fore/aft displacement
    uint32_t   foreaft_displacement;
    // Left Motor Torque
    int16_t    left_torque;
    // Right Motor Torque
    int16_t    right_torque;
    // Servo Frames
    uint16_t   servo_frames;
    // User Interface battery voltage
    uint16_t   ui_battery_voltage;
    // User Interface battery status
    //uint16_t   ui_battery_status;
    // Powerbase attery Voltage (min of A, B)
    uint16_t   base_battery_voltage;
    // Operational Mode (0= disabled, 1=tractor, 2= balance)
    uint16_t operational_mode;
    // Controller Gain Schedule (0=normal, 1=tall, 2=heavy)
    uint16_t controller_gain_schedule;
    // Velocity command (as received)
    int16_t received_velocity_command;
    // Turn Command (as received)
    int16_t received_turnrate_command;

    // CU Status "word 1"
    int16_t status_word1;
    // CU Status "word 2"
    int16_t status_word2;

private: 

    std::string CuStatus1ToString() const;
    std::string CuStatus2ToString() const;

    // Track which messages we've received
    bool haveMonitorMsg_[8];
    bool haveStatusMsg_;

    orcaice::Context context_;

};

}

#endif
