/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_LASER2D_HW_DRIVER_H
#define ORCA2_LASER2D_HW_DRIVER_H

#include <string>
#include <orca/orca.h>

namespace laser2d {

//!
//! @brief Abstract class for a scanning laser.
//! 
//! All laser scanners should implement this, making it simple
//! to add support for different laser scanners.
//! 
//! This guy is _not_ guaranteed to be thread-safe!
//!
//! Note: Driver should _not_ throw exceptions from the constructor.
//!
//! @author Alex Brooks a.brooks at cas.edu.au
//!
class Driver 
{

public:

    class Config
    {   
    public:
        Config();
        bool validate() const;
        std::string toString() const;
        bool operator==( const Config & other );
        bool operator!=( const Config & other );

        double minRange;
        double maxRange;
        double fieldOfView;
        double startAngle;
        int    numberOfSamples;
    };

    virtual ~Driver() {};

    // Blocks till new data is available, but shouldn't occupy the thread indefinitely.
    virtual void read( float *ranges, unsigned char *intensities, orca::Time &timeStamp )=0;

protected:

};

}

#endif
