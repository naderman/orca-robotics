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

#ifndef ORCA2_STAGE_LOCALISER_STATIC_DRIVER_H
#define ORCA2_STAGE_LOCALISER_STATIC_DRIVER_H

#include "driver.h"

namespace simlocaliser
{

class StaticDriver : public Driver
{
public:

    StaticDriver( const orca::Frame2d & pose );
    virtual ~StaticDriver() {}; 

    // returns: 0 = success, non-zero = failure
    virtual int enable() { return 0; };
    virtual int repair() { return 0; };
    virtual int disable() { return 0; };

    virtual int read( orca::Localise2dDataPtr & location2d );

private:

    orca::Localise2dDataPtr location2d_;
};

} // namespace

#endif
