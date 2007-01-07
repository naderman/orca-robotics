/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/input.h>
#include <string.h>
#include <errno.h>

#include "evdevutil.h"

/* this macro is used to tell if "bit" is set in "array"
 * it selects a byte from the array, and does a boolean AND 
 * operation with a byte that only has the relevant bit set. 
 * eg. to check for the 12th bit, we do (array[1] & 1<<4)
 */
#define test_bit(bit, array)    (array[bit/8] & (1<<(bit%8)))

namespace teleop
{

void
printDeviceInfo( const char *dev )
{
    int fd = -1;
    struct input_id device_info;

    /* ioctl() requires a file descriptor, so we check we got one, and then open it */
    if ((fd = open(dev, O_RDONLY)) < 0) {
        perror("evdev open");
        return;
    }

    /* suck out some device information */
    if(ioctl(fd, EVIOCGID, &device_info)) {
        perror("evdev ioctl");
        return;
    }

    /* the EVIOCGID ioctl() returns input_id
     * structure - see <linux/input.h> 
     * So we work through the various elements, displaying 
     * each of them 
     */
    printf("vendor 0x%04hx product 0x%04hx version 0x%04hx is on",
           device_info.vendor, device_info.product, 
           device_info.version);
    switch ( device_info.bustype)
    {
    case BUS_PCI :
        printf(" a PCI bus\n");
        break;
    case BUS_ISAPNP :
        printf(" a Plug-n-pray ISA bus\n");
        break;
    case BUS_USB :
        printf(" a Universal Serial Bus\n");
        break;
    case BUS_ISA :
        printf(" a legacy ISA bus\n");
        break;
    case BUS_I8042 :
        printf(" an I8042 (or similar) controller\n");
        break;
    case BUS_XTKBD :
        printf(" a IBM XT bus\n");
        break;
    case BUS_RS232 :
        printf(" a RS232 serial bus\n");
        break;
    case BUS_GAMEPORT :
        printf(" a gameport\n");
        break;
    case BUS_PARPORT :
        printf(" a parallel port\n");
        break;
    case BUS_AMIGA :
        printf(" an Amiga unique interface\n");
        break;
    case BUS_ADB :
        printf(" an Apple Desktop Bus\n");
        break;
    case BUS_I2C :
        printf(" a inter-integrated circuit bus\n");
        break;
    default:
        printf(" an unknown bus type: 0x%04hx\n", device_info.bustype);
    }

    close(fd);
}

void
printInputType( const char *dev )
{
    int fd = -1;
    uint8_t evtype_bitmask[EV_MAX/8 + 1];
    int yalv;

    /* ioctl() requires a file descriptor, so open it */
    if ((fd = open(dev, O_RDONLY)) < 0) {
        perror("evdev open");
        return;
    }

    memset(evtype_bitmask, 0, sizeof(evtype_bitmask));
    if (ioctl(fd, EVIOCGBIT(0, EV_MAX), evtype_bitmask) < 0) {
        perror("evdev ioctl");
        return;
    }

    printf("Supported event types:\n");

    for (yalv = 0; yalv < EV_MAX; yalv++) {
        if (test_bit(yalv, evtype_bitmask)) {
            /* this means that the bit is set in the event types list */
            printf("  Event type 0x%02x ", yalv);
            switch ( yalv)
            {
            case EV_KEY :
                printf(" (Keys or Buttons)\n");
                break;
            case EV_REL :
                printf(" (Relative Axes)\n");
                break;
            case EV_ABS :
                printf(" (Absolute Axes)\n");
                break;
            case EV_MSC :
                printf(" (Something miscellaneous)\n");
                break;
            case EV_LED :
                printf(" (LEDs)\n");
                break;
            case EV_SND :
                printf(" (Sounds)\n");
                break;
            case EV_REP :
                printf(" (Repeat)\n");
                break;
            case EV_FF :
                printf(" (Force Feedback)\n");
            default:
                printf(" (Unknown event type: 0x%04hx)\n", yalv);
            }		 
        }
    }

    close(fd);
}



int
devIsUSB( const char *dev, bool &isUSB )
{
    int fd = -1;
    struct input_id device_info;
    isUSB = false;

    /* ioctl() requires a file descriptor, so we check we got one, and then open it */
    if ((fd = open(dev, O_RDONLY)) < 0)
    {
        perror("evdev open");
        return errno;
    }

    /* suck out some device information */
    if(ioctl(fd, EVIOCGID, &device_info))
    {
        perror("evdev ioctl");
        close(fd);
        return errno;
    }

    /* the EVIOCGID ioctl() returns input_id
     * structure - see <linux/input.h> 
     */
    if ( device_info.bustype == BUS_USB )
    {
        close(fd);
        isUSB = true;
        return 0;
    }

    close(fd);
    return 0;
}


int
devSupportsAbsoluteAxes( const char *dev, bool &supportsAbsoluteAxes )
{
    int fd = -1;
    uint8_t evtype_bitmask[EV_MAX/8 + 1];
    int yalv;
    int ret;
    supportsAbsoluteAxes = false;

    /* ioctl() requires a file descriptor, so open it */
    if ((fd = open(dev, O_RDONLY)) < 0) 
    {
        ret=errno;
        // perror("Couldnt open device");
        return ret;
    }

    memset(evtype_bitmask, 0, sizeof(evtype_bitmask));
    if (ioctl(fd, EVIOCGBIT(0, EV_MAX), evtype_bitmask) < 0) 
    {
        ret=errno;
        // perror("evdev ioctl");
        close(fd);
        return ret;
    }

    for (yalv = 0; yalv < EV_MAX; yalv++) {
        if (test_bit(yalv, evtype_bitmask)) {
            /* this means that the bit is set in the event types list */
            if ( yalv == EV_ABS )
            {
                close(fd);
                supportsAbsoluteAxes = true;
                return 0;
            }
        }
    }

    close(fd);
    return 0;
}

int
devName( const char *dev, char *name, int nameSize )
{
    int fd = -1;

    /* ioctl() requires a file descriptor, so open it */
    if ((fd = open(dev, O_RDONLY)) < 0) 
    {
        perror("Couldnt open device");
        return -1;
    }

    if ( ioctl( fd, EVIOCGNAME(nameSize), name ) < 0 ) 
    {
        perror("evdev ioctl");
        close(fd);
        return -1;
    }

    return 0;
}

} // namespace
