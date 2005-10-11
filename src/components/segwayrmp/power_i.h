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

#ifndef ORCA_POWER_I_H
#define ORCA_POWER_I_H

// include defnition of Ice runtime
#include <Ice/Ice.h>

// include provided interfaces
#include <orca/power.h>

// utilities
#include <orca/ptrproxy.h>


class PowerI : public orca::Power
{
    public:
        PowerI( orca::util::PtrProxy* power );

        virtual ::orca::PowerDataPtr getData(const ::Ice::Current& );

        // the driver will put the latest data into this proxy
        orca::util::PtrProxy* power_;
};

#endif
