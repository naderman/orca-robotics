/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Jose Guivant, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASER2D_SICK_ACFR_LASER_H
#define ORCA2_LASER2D_SICK_ACFR_LASER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sickdefines.h"

struct Laser
{
    struct sic_packet *sp ;			//pointer to data buffer
	char *NameUnit ;			// unit logical name
	char *NameCB ;				// circular buffer name	(old)
	unsigned char code ;			
	char *sufixFile ;			// name file in recorder service (old)
	int MaxFileSize ;			// max file size in recorder service1 (old)
	struct reco *rr_lsr;			// CB instance
	int ignoredx,ignoredT,usedChars,FramesOK,BadCrcs;	//some stats
	xuint32 TimeOuts;
	int FComLsr;				// handle to serial device
	int timeStamp ;
	pthread_t tid ;
	int priority,priority2 ;		// priorities in stage1 and 2
	pthread_t wdid;
	struct threadData ThreadData ;
	int active,saving,used ;
	// FILE *FFileLsr ;
	char NamePort[20] ;                    // name of serial :example  "/dev/ser1" ..etc
	int speed,speedB ;
	int init ;
	int PublishFlag ;	               // setting for DB publishing
	int hLHistoric;		               // handle to DB service : 1 QUEUE instance
	int simu ;			       // simulation mode?
	xuint32 DtMinTx,LastTxTime;
	int IgnoredFramesB;
	int cx ;
	int CxReset,CxReset2 ;
	int FlagBye ;
};	

void* IniLaserInstance(unsigned int n, int speed0, int speed2, int init, int serialPort)
;

int IniLSRXCode(int* pflag, struct Laser* pxl)
;

void SetLaser(int FComLsr, struct Laser* pg)
;

// static void PushLHLaserFrame(struct Laser *pg);

#define RANGESIZE	361

// sick laser packet
struct sic_packet		// laser packet
{	xuint32 timestamp;	// timestamp (in millisconds)
	xuint16 cx ;
	xuint16 range[RANGESIZE];	// range+intensity data
	xuint16 BadCrcs ;			// (0 to 180 deg in 0.5 deg steps)
	xuint16 Status ;
};

#ifdef __cplusplus
}
#endif


#endif

