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

#include <orcarobotdriverutil/ihwdriver.h>
#include "types.h"

namespace segwayrmp
{

//
// Abstract interface class for something that talks to the RMP
// hardware (or a simulator).
//
// Member functions throw RmpExceptions on error conditions.
//
typedef orcarobotdriverutil::IHwDriver<Command,Data> HwDriver;

} // namespace

#endif
