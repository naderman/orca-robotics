/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_RMP_USB_DATA_FRAME_H
#define ORCA2_RMP_USB_DATA_FRAME_H

namespace robot2d
{

class CanPacket;

// this holds all the RMP data it gives us
class RmpUsbDataFrame
{
public:
    RmpUsbDataFrame();

    // Adds information from a packet
    void AddPacket(const CanPacket* pkt);

    // Print out frame data to string
    std::string toString();

    // Allows adding more packets to the frame without resetting the data
    void reopen();

    // Resets all data to RMP_CAN_DROPPED_PACKET
    void reset();

    // Did the next frame start?
    bool isClosed() const;
    // Did all the packets arrive?
    bool isComplete() const;
    
    std::string CuStatus1ToString();
    std::string CuStatus2ToString();

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
    uint32_t   yaw;
    // Yaw rate
    int16_t    yaw_dot;
    // wheel displacement left
    uint32_t   left;
    // Left wheel Velocity
    int16_t    left_dot;
    // wheel displacement right
    uint32_t   right;
    // Right wheel Velocity
    int16_t    right_dot;
    // fore/aft displacement
    uint32_t   foreaft;
    // Left Motor Torque
    int16_t    left_torque;
    // Right Motor Torque
    int16_t    right_torque;
    // Servo Frames
    uint16_t   frames;
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
    int16_t velocity_command;
    // Turn Command (as received)
    int16_t rate_command;

    // CU Status "word 1"
    int16_t status_word1;
    // CU Status "word 2"
    int16_t status_word2;

private:

    bool msgCheckList_[8];
    bool statusCheck_;
    
    //debug
    bool kill_flag;
    // ui battery info from the heartbeat msg
    uint16_t   ui_heartbeat_voltage;
    uint16_t   ui_heartbeat_status;

};

} // namespace

#endif
