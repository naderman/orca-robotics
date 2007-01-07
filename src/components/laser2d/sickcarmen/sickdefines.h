/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
/* AlexB: This code was forked from CARMEN then overhauled */
/* Tobi: Modified to make it work for 500kbps over RS422 */

#ifndef SICKDEFINES_H
#define SICKDEFINES_H

#include <string>
#include <assert.h>

#define LASER_BUFFER_SIZE                100000

#define CRC16_GEN_POL                    0x8005
#define CRC16_GEN_POL0                   0x80
#define CRC16_GEN_POL1                   0x05

#define LMS_PASSWORD                     "SICK_LMS"
#define PLS_PASSWORD                     "SICK_PLS"

#define BUFFER_SIZE                      16000
#define MAX_COMMAND_SIZE                 8196
#define MAX_NAME_LENGTH                  256

#define MAX_TIME_FOR_CLEAR               0.2
#define MAX_TIME_FOR_DATA                0.3
#define MAX_TIME_FOR_ACK                 0.1
#define MAX_TIME_FOR_ANSWER              0.1
#define MAX_TIME_FOR_SENSOR_DATA         0.5
// Tobi: changed MAX_TIME_FOR_CONFIG from 3.0 to 5.0 so that sick_set_config_mode()
// could change from 500kbps to the slower baud rates 
#define MAX_TIME_FOR_CONFIG              5.0 
#define MAX_TIME_FOR_GETTING_CONF        0.3
#define MAX_TIME_FOR_TESTING_BAUDRATE    1.0

#define INI                              -1
#define TIO                              0
#define STX                              0x02
#define UKN                              0x05
#define ACK                              0x06
#define DLE                              0x10
#define NAK                              0x15
#define LID                              0x80

#define RES_1_00_DEGREE                  0
#define RES_0_50_DEGREE                  1 
#define RES_0_25_DEGREE                  2

#ifndef TIOCGETP 
#define TIOCGETP                         0x5481
#define TIOCSETP                         0x5482
#define RAW                              1
#define CBREAK                           64
#endif

typedef enum { PLS, LMS } laser_model_t;
typedef enum { CM, MM, DM } range_res_t;
typedef enum { RANGE80M, RANGE160M, RANGE320M } range_dist_t;
typedef enum { N, E, O } parity_t;
    
class laser_device_t {
public:
    int                fd;
    laser_model_t      type;
    std::string        ttyport;
    int                baudrate;
    parity_t           parity;
    std::string        passwd;
    int                databits;
    int                stopbits;
    int                hwf;
    int                swf;
    int                laser_num;
};

typedef laser_device_t *laser_device_p;
    
class laser_settings_t {
public:

    // May throw std::strings on dodgy settings.
    laser_settings_t( const char *device, 
                      const char *laserType, // eg "LMS"
                      int         baudrate,
                      double      resolutionInDegrees,
                      range_res_t rangeResolution );

    std::string   device_name;
    laser_model_t type;
    range_res_t   range_res;
    range_dist_t  range_dist;
    std::string   password;
    int           laser_num;
    int           detect_baudrate, use_highspeed;
    int           start_baudrate, set_baudrate;
    int           databits, stopbits;
    parity_t      parity;
    int           swf, hwf;
    int           angle_range, angle_resolution;
    int           num_values;
    int           laser_flipped;

    // Throws std::strings if something's fucked up
    void checkValid() const;

    // for debugging
    void print() const;
};

#endif
