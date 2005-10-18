/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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

#ifndef ORCA2_TELEOP_INPUT_DRIVER_INTERFACE_H
#define ORCA2_TELEOP_INPUT_DRIVER_INTERFACE_H

#include <orca/platform2d.h>

/*!

@brief Abstract interface class for human teleoperation input.

@author Alex Makarenko

*/
class InputDriver
{

public:

    virtual int enable()=0;
    virtual int disable()=0;

    //! Blocks till new data is available
    virtual int readdata( orca::Velocity2dCommandPtr &data )=0;

    //! Set a specifc configuration
    //virtual int setConfig( const orca::LaserConfigPtr &cfg )=0;

    //! Get the current configuration
    //virtual int getConfig( orca::LaserConfigPtr &cfg )=0;

};

#endif
