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

#ifndef ORCA2_SEGWAY_RMP_USB_DRIVER_CONFIG_H
#define ORCA2_SEGWAY_RMP_USB_DRIVER_CONFIG_H

#include <string>
#include <orcaice/context.h>

namespace robot2d
{

//
// Convenience structure to hold all the config parameters.
// All units are S.I.
//
class UsbDriverConfig 
{
public:

    UsbDriverConfig();

    int gainSchedule;
    double maxVelocityScale;
    double maxTurnrateScale;
    double maxAccelerationScale;
    double maxCurrentLimitScale;

    // returns 0 if everything looks good.
    int checkSanity( std::string &warnings, std::string &errors );
};

std::ostream &operator<<( std::ostream & s, const UsbDriverConfig & c );

void readFromProperties( const orcaice::Context & context, UsbDriverConfig & c );

} // namespace

#endif
