/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_HARDWARE_DRIVER_H
#define ORCA2_SEGWAY_RMP_HARDWARE_DRIVER_H

namespace segwayrmp
{

class HwDriver
{

public:

    struct SegwayRmpData
    {
        int seconds;
        int useconds;
        double x;
        double y;
        double roll;
        double pitch;
        double yaw;
        double vx;
        double droll;
        double dpitch;
        double dyaw;
        double mainvolt;
        double uivolt;
    };

    struct SegwayRmpCommand
    {
        double vx;
        double w;
    };

    struct SegwayRmpStats
    {
        double distanceTravelled;
    };

    virtual ~HwDriver() {};
    
    // Returns 0 on success. Does not throw.
    virtual int enable()=0;

    // Returns 0 on success. Does not throw.
    virtual int disable()=0;

    // Blocking read. Returns 0 on success. Does not throw.
    virtual int read( SegwayRmpData& data, std::string &status )=0;

    // Writes velocity command. Returns 0 on success. Does not throw.
    virtual int write( const SegwayRmpCommand& command )=0;

    // Non-blocking read of secondary data. Returns 0 on success.
    virtual int get( SegwayRmpStats& stats ) { return 0; };

    // For debugging, convert to string as much of internal state as possible
    virtual std::string toString() { return std::string(""); };

};

} // namespace

#endif
