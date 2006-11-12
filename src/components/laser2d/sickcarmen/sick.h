/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef SICK_H
#define SICK_H

#include "sickdefines.h"
#include <assert.h>

class sick_laser_t {
public:

    // May throw std::strings with dodgy settings
    sick_laser_t( const laser_settings_t &settings );
    ~sick_laser_t();

    laser_settings_t  settings;
    laser_device_t    dev;

    int               numvalues;
    double           *range;
    int              *glare, *wfv, *sfv;
    unsigned char    *buffer;
    long int          buffer_position, processed_mark, packet_offset, packet_length;
    int               new_reading;
    double            timestamp;

private:

    // Don't be copying this thing
    void operator=(const sick_laser_t &);
    sick_laser_t(const sick_laser_t &);
};

typedef sick_laser_t *sick_laser_p;

int sick_start_laser(sick_laser_p laser);
int sick_stop_laser(sick_laser_p laser);
void sick_handle_laser(sick_laser_p laser);

char *sick_info();

#endif
