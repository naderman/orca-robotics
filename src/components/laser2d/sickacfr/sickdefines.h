/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Jose Guivant, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASER2D_SICK_ACFR_SICKDEFINES_H
#define ORCA2_LASER2D_SICK_ACFR_SICKDEFINES_H

// a pointer to a function which has a void* argument and returns a void*
typedef  void* (*FunciThread)(void* )  ;

#define  uchar   unsigned char
#define  uint16b unsigned short int
#define  int16b  short int
#define  int16   short int
#define  uint    unsigned int
#define  uint8   uint8_t 
#define  uint16  uint16_t 
#define  uint8b  unsigned char
#define  uint32  unsigned int
#define  int32  int


#define PIs180  (3.141592/180.0)
struct threadData	{	int *pflag ; int priority ; } ;	
#define T5segs  50

struct TaskXX {	char active,priority,saving ; int xx[5] ; char name[15] ; char *DevSer ; } ;

#define XXprintf printf

extern void Msg(char *s) ;
extern void PushMsg(int h,int x,int y,char *msg);
extern void GlobalMsg(char *s);

// -------------------------------------------------------------------
extern char *GivemeStrEntry(char *head) ;
// -------------------------------------------------------------------

// --------------------------------------------------------
#define xuint8   unsigned char 
#define xint8    signed   char 
#define xuint16  unsigned short int 
#define xint16   signed   short int  
#define xuint32  unsigned long int 
#define xint32   signed long int  
#define xuint    unsigned int  

// ...........................................

// #include "serial.h" 

#endif
