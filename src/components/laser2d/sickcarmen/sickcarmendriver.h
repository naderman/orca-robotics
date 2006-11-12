/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef NATIVELASERDRIVER_H
#define NATIVELASERDRIVER_H

#include <driver.h>
#include "sick.h"
#include <orcaice/context.h>

namespace sicklaser {

//
// @author Alex Brooks
//
class SickCarmenDriver : public Driver
{

public: 

    SickCarmenDriver( const char       *device,
                      const char       *laserType,
                      const int         baudrate,
                      orcaice::Context  context );
    ~SickCarmenDriver();

    virtual int enable();
    virtual int disable();

    virtual bool isEnabled();

    // Blocks till new data is available
    virtual int read( orca::LaserScanner2dDataPtr &data );

    // Set a specifc configuration
    virtual int setConfig( const orca::RangeScanner2dConfigPtr &cfg );

    // Get the current configuration
    virtual int getConfig( orca::RangeScanner2dConfigPtr &cfg );

    virtual const std::string heartbeatMessage();

private: 

    int setupParams( double angleIncrement, double rangeResolution, int baudrate );

    // these versions don't clear infoMessage_ first.
    int doEnable();
    int doDisable();

    bool enabled_;
    sick_laser_t *laser_;

    char *device_;
    
    // LMS or PMS
    char *type_; 
    
    const int baudrate_;

    orcaice::Context context_;

    orca::RangeScanner2dConfigPtr currentConfig_;

    char sickInfoMessage_[2000];

    bool   firstRead_;
    double lastStatsUpdateTime_;
    bool   laserStalled_;
};

}

#endif
