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
#include <orcaobj/timeutils.h>

#include "sickdefines.h"
#include "serial.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RANGESIZE	361

// sick laser packet
struct sic_packet		
{	
	// stores timestamp after the first byte of the laser frame arrives
	orca::Time timeStamp;
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
	// pointer to raw laser data 
    struct sic_packet *sp ;		
	// unit logical name 
	char *NameUnit ;			
	// circular buffer name	(old)
	char *NameCB ;				
	unsigned char code ;			
	// name file in recorder service (old)
	char *sufixFile ;			
	// max file size in recorder service1 (old)
	int MaxFileSize ;			
	// CB instance
	struct reco* rr_lsr;		   
	int ignoredx,ignoredT,usedChars,FramesOK,BadCrcs;	//some stats
	xuint32 TimeOuts;
    // file descriptor for an opened serial device
	int FComLsr;				
	int timeStamp ;
	pthread_t tid ;
	// initial priority of this thread
    int priority;
	// dynamically adjust the thread to this priority 
	int priority2;	   
	pthread_t wdid;
	struct threadData ThreadData ;
	int active,saving,used ;
	// FILE *FFileLsr ;
	char NamePort[20] ;                    // name of serial :example  "/dev/ser1" ..etc
	// initial baudrate
	int speed;
	// desired baudrate
	int speedB ;
	int init ;
	int PublishFlag ;	               // setting for DB publishing
	int hLHistoric;		               // handle to DB service : 1 QUEUE instance
	int simu ;			       // simulation mode?
	xuint32 DtMinTx;
    double LastTxTime;
	int IgnoredFramesB;
	int cx ;
	int CxReset,CxReset2 ;
	// flag which is checked by thread indicating whether it should stop
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
void parse_SICK_LASER(struct sic_packet *pxxx,unsigned char *string,struct reco *rr_lsr, orca::Time timeStamp, int *BadCrcs,struct LaserData *pxl);

void PutCRCinStr(char *str,int size); 

void readResponseInfo1(int FComLsr);

void readResponseB(int FComLsr,struct LaserData *pg);

void readResponseC(int FComLsr,struct LaserData *pg);

void readResponse(int FComLsr);

void readSickLaser(struct LaserData *pxl,int *pFlag);


uint16b ExeCRC(unsigned char *pf,int n,uint16b crc_poly);

void ReadRangeData2_U16(uint16b *pf);

laser2d::Serial* serial_;
 
};

#endif

