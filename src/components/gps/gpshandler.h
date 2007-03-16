/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GPS_HANDLER_H
#define ORCA2_GPS_HANDLER_H

#include <orca/gps.h>
#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>
#include <orcagpsutil/mapgrid.h>

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

    orcagpsutil::mgaMapgrid mgaMapgrid_;

    orcaice::Context context_;
};

#endif
