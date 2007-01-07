/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Jose Guivant, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASER2D_SICK_ACFR_SERIAL_H
#define ORCA2_LASER2D_SICK_ACFR_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------
// extern int SerialWriteJEG(unsigned H,uchar *buff,int L,int *nw);
// extern void CloseSerialPortJEG( unsigned *H) ;
// extern int OpenSerialPortXJEG(char *name,int speed0, unsigned *H);
// extern int SerialReadJEG(unsigned h, uchar *buff, int lMin, int lMax, int TimeOut);
// extern int SerialWaitForCharJEG(unsigned h, uchar b ,int timeoutDs,int *ignoredChars );
// extern int	ChangeSerialSpeedJEG(unsigned H,int speed);
// extern int SerialReadUntilCharXJEG(unsigned h,uchar *string,int Lmax,uchar b,int timeoutDs,int *ignoredChars);
// extern int CharsWaitingJEG(unsigned h);
// extern int SerialWaitForCharXJEG(unsigned h, uchar b ,int timeoutDs,int *ignoredChars ) ;
// --------------------------------------------------------

int SerialWriteJEG(unsigned H,uchar *buff,int L,int *nw);
void CloseSerialPortJEG( unsigned *H) ;
int OpenSerialPortXJEG(char *name,int speed0, unsigned *H);
int SerialReadJEG(unsigned h, uchar *buff, int lMin, int lMax, int TimeOut);
int SerialWaitForCharJEG(unsigned h, uchar b ,int timeoutDs,int *ignoredChars );
int	ChangeSerialSpeedJEG(unsigned H,int speed);
int SerialReadUntilCharXJEG(unsigned h,uchar *string,int Lmax,uchar b,int timeoutDs,int *ignoredChars);
int CharsWaitingJEG(unsigned h);
int SerialWaitForCharXJEG(unsigned h, uchar b ,int timeoutDs,int *ignoredChars ) ;

#ifdef __cplusplus
}
#endif

#endif
