/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 NovAtel Inc.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef _HEADER_H_
#define _HEADER_H_

// This structure defines the binary header on every OEM4 log
namespace novatel {

#pragma pack(push,1)
typedef struct OEM4_BINARY_HEADER         // Standard binary header
{
   unsigned char          sop1;          // start of packet first byte  (0xaa)
   unsigned char          sop2;          // start of packet second byte (0x44)
   unsigned char          sop3;          // start of packet third  byte (0x12)
   unsigned char          header_length; // Length of the header
   unsigned short         number;        // Message number
   unsigned char          type;          // Message type
   unsigned char          port_address;  // Address of the data port the log was received on
   unsigned short         length;        // Message length
   unsigned short         sequence;      // Sequence #
   unsigned char          idle;          // Idle time
   unsigned char          gps_stat;      // GPS Time Status 
   unsigned short         gps_week;      // GPS Week number
   unsigned long          millisecs;     // Milliseconds into week
   unsigned long          status;        // Receiver status word
   unsigned short         crc;           // The 16bit CRC check sum
   unsigned short         version;       // Receiver software version
}OEM4_BINARY_HEADER;

typedef struct OEM4_SHORT_BINARY_HEADER         // Short binary header
{
   unsigned char          sop1;          // start of packet first byte  (0xaa)
   unsigned char          sop2;          // start of packet second byte (0x44)
   unsigned char          sop3;          // start of packet third  byte (0x13)
   unsigned char          length;        // Message length
   unsigned short         number;        // Message Number
   unsigned short         gps_week;      // GPS Week number
   unsigned long          millisecs;     // Milliseconds into week
}OEM4_SHORT_BINARY_HEADER;
#pragma pack(pop)

enum TIME_STATUS
{
   GPSTIME_UNKNOWN = 0,
   GPSTIME_USER,
   GPSTIME_USERADJUSTING,
   GPSTIME_COARSEADJUSTING,
   GPSTIME_COARSE,
   GPSTIME_COARSESTEERING,
   GPSTIME_FINEADJUSTING,
   GPSTIME_FINE,
   GPSTIME_FINESTEERING,
   GPSTIME_FREEWHEELING,
   GPSTIME_SATTIME,
};

enum PORT_NAME 
{
   NO_PORTS = 0,
   COM1_ALL = 1,
   COM2_ALL = 2,
   COM3_ALL = 3,
   USB_ALL = 4,
   THISPORT_ALL = 6,
   ALLPORTS = 8,
   COM1 = 32,
   COM1_1,
   COM1_2,
   COM1_3,
   COM1_4,
   COM1_5,
   COM1_6,
   COM1_7,
   COM1_8,
   COM1_9,
   COM1_10,
   COM1_11,
   COM1_12,
   COM1_13,
   COM1_14,
   COM1_15,
   COM1_16,
   COM1_17,
   COM1_18,
   COM1_19,
   COM1_20,
   COM1_21,
   COM1_22,
   COM1_23,
   COM1_24,
   COM1_25,
   COM1_26,
   COM1_27,
   COM1_28,
   COM1_29,
   COM1_30,
   COM1_31,
   COM2 = 64,
   COM2_1,
   COM2_2,
   COM2_3,
   COM2_4,
   COM2_5,
   COM2_6,
   COM2_7,
   COM2_8,
   COM2_9,
   COM2_10,
   COM2_11,
   COM2_12,
   COM2_13,
   COM2_14,
   COM2_15,
   COM2_16,
   COM2_17,
   COM2_18,
   COM2_19,
   COM2_20,
   COM2_21,
   COM2_22,
   COM2_23,
   COM2_24,
   COM2_25,
   COM2_26,
   COM2_27,
   COM2_28,
   COM2_29,
   COM2_30,
   COM2_31,
   COM3 = 96,
   COM3_1,
   COM3_2,
   COM3_3,
   COM3_4,
   COM3_5,
   COM3_6,
   COM3_7,
   COM3_8,
   COM3_9,
   COM3_10,
   COM3_11,
   COM3_12,
   COM3_13,
   COM3_14,
   COM3_15,
   COM3_16,
   COM3_17,
   COM3_18,
   COM3_19,
   COM3_20,
   COM3_21,
   COM3_22,
   COM3_23,
   COM3_24,
   COM3_25,
   COM3_26,
   COM3_27,
   COM3_28,
   COM3_29,
   COM3_30,
   COM3_31,
   USB = 128,
   USB_1,
   USB_2,
   USB_3,
   USB_4,
   USB_5,
   USB_6,
   USB_7,
   USB_8,
   USB_9,
   USB_10,
   USB_11,
   USB_12,
   USB_13,
   USB_14,
   USB_15,
   USB_16,
   USB_17,
   USB_18,
   USB_19,
   USB_20,
   USB_21,
   USB_22,
   USB_23,
   USB_24,
   USB_25,
   USB_26,
   USB_27,
   USB_28,
   USB_29,
   USB_30,
   USB_31,
   THISPORT = 192,
   THISPORT_1, 
   THISPORT_2, 
   THISPORT_3, 
   THISPORT_4, 
   THISPORT_5, 
   THISPORT_6, 
   THISPORT_7, 
   THISPORT_8, 
   THISPORT_9, 
   THISPORT_10,
   THISPORT_11,
   THISPORT_12,
   THISPORT_13,
   THISPORT_14,
   THISPORT_15,
   THISPORT_16,
   THISPORT_17,
   THISPORT_18,
   THISPORT_19,
   THISPORT_20,
   THISPORT_21,
   THISPORT_22,
   THISPORT_23,
   THISPORT_24,
   THISPORT_25,
   THISPORT_26,
   THISPORT_27,
   THISPORT_28,
   THISPORT_29,
   THISPORT_30,
   THISPORT_31,
   MAX_PORT
};


}//namespace novatel

#endif
