/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_SEGWAY_RMP_HARDWARE_DRIVER_H
#define ORCA2_SEGWAY_RMP_HARDWARE_DRIVER_H

#include <orca/platform2d.h>
#include <orca/power.h>

namespace robot2d
{

/*
 * @brief Abstract interface class to robotic base.
 * @author Alex Makarenko
*/
class HwDriver
{

public:
    virtual ~HwDriver() {};
    
    // Returns 0 on success. Does not throw.
    virtual int enable()=0;
    
    // Returns 0 on success. Does not throw.
    virtual int repair()=0;
    
    // Returns 0 on success. Does not throw.
    virtual int disable()=0;

    // Blocking read. Returns 0 on success. Does not throw.
    virtual int read( orca::Position2dDataPtr &position2d, orca::PowerDataPtr &power, std::string &status )=0;

    // Writes velocity command. Returns 0 on success. Does not throw.
    virtual int write( const orca::Velocity2dCommandPtr & command )=0;

    // For debugging, convert to string as much of internal state as possible
    virtual std::string toString() { return std::string(""); };

};

} // namespace

#endif
