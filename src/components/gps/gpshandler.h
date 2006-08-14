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
#ifndef ORCA2_GPS_HANDLER_H
#define ORCA2_GPS_HANDLER_H

#include <orca/gps.h>
#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>
#include <orcanavutil/mapgrid.h>

#include "gpsdriver.h"
#include "gpsI.h"

//
// @brief the main handler of this GPS component.
//
// Note: this thing self-destructs when run() returns.
//
class GpsHandler : public orcaice::Thread
{

public:

    GpsHandler(GpsI             &gpsObj,
	       GpsDriver        *hwDriver,
	       orcaice::Context current,
	       bool             startEnabled );
    ~GpsHandler();

    virtual void run();

private:
    GpsI &gpsObj_;
    // Generic driver for the hardware
    GpsDriver *hwDriver_;

    mgaMapgrid mgaMapgrid_;

    orcaice::Context context_;
};

#endif
