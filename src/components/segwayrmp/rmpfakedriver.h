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

#ifndef ORCA2_SEGWAY_RMP_FAKE_DRIVER_H
#define ORCA2_SEGWAY_RMP_FAKE_DRIVER_H

#include "rmpdriver.h"

/*!
    A fake driver to simplify development. Does not require any hardware.
 */
class RmpFakeDriver : public RmpDriver
{
public:

    RmpFakeDriver();
    virtual ~RmpFakeDriver();

    virtual int enable();
    virtual int disable();

    //! Blocks till new data is available
    virtual int read( orca::Position2dDataPtr & position2d, orca::PowerDataPtr & power );

    virtual int sendMotionCommand( orca::Velocity2dCommandPtr & command );

    virtual int resetIntegrators() { return 0; };
};

#endif
