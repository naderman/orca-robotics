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

// buffer for laser data
#include <orcaice/buffer.h>
 
#include "sickdefines.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RANGESIZE	361

// sick laser packet
struct sic_packet		// laser packet
{	xuint32 timestamp;	// timestamp (1 = 10 microseconds)
	xuint16 cx ;
	// range and intensity data
	// hard coded for 0 to 180 deg in 0.5 deg steps
	// first 13 bytes are range
	// last 3 bytes are intensity
	xuint16 range[RANGESIZE];	
	xuint16 BadCrcs ;			
	xuint16 Status ;
};

// struct to hold laser data and parameters
struct LaserData
{
    struct sic_packet *sp ;			//pointer to data buffer
	char *NameUnit ;			// unit logical name
	char *NameCB ;				// circular buffer name	(old)
	unsigned char code ;			
	char *sufixFile ;			// name file in recorder service (old)
	int MaxFileSize ;			// max file size in recorder service1 (old)
	struct reco* rr_lsr;			// CB instance
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
 
#ifdef __cplusplus
}
#endif
 
class Laser {

public:

Laser();
~Laser();

struct sic_packet FrameLsr1 ;
struct LaserData* pxl;

// initialise a laser 
void* IniLaserInstance(unsigned int n, int speed0, int speed2, int init, int serialPort);

void* readSickLaserX(void *pxl);

orcaice::Buffer<struct LaserData> laserDataBuffer_;

private:

// spawn a thread for receiving laser packets
int IniLSRXCode(int* pflag, struct LaserData* pxl);

void SetLaser(int FComLsr, struct LaserData* pg);

// parse the incoming laser packet and push into a buffer for publishing
void parse_SICK_LASER(struct sic_packet *pxxx,unsigned char *string,struct reco *rr_lsr,xuint32 timestamp,int *BadCrcs,struct LaserData *pxl);

void PutCRCinStr(char *str,int size); 

void readResponseInfo1(int FComLsr);

void readResponseB(int FComLsr,struct LaserData *pg);

void readResponseC(int FComLsr,struct LaserData *pg);

void readResponse(int FComLsr);

void readSickLaser(struct LaserData *pxl,int *pFlag);


uint16b ExeCRC(unsigned char *pf,int n,uint16b crc_poly);

void ReadRangeData2_U16(uint16b *pf);

 
};

#endif

