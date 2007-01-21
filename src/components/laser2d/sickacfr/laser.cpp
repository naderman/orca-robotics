/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Jose Guivant, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */


#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include <orcaobj/stringutils.h>
#include <orcaserial/serialutil.h>

#include "sickdefines.h"
#include "time.h"
// #include "serial.h"
#include "laser.h"

//TODO: Get rid of FComLsr... it's not actually being used anymore


static uint LaserSensibilityMode=0 ;
// ...........................................................

Laser::Laser() 
	: pxl(0),
      serial_(0)
{
	serial_ = new orcaserial::Serial();
	
    // configure the buffers so they have depth 100 and are of type queue
    laserDataBuffer_.configure( 100 , orcaice::BufferTypeCircular );
 
}

Laser::~Laser() 
{
   // wait until the thread has stopped
   if ( pthread_cancel( pxl->tid ) == EOK)
   {
		// wait until the thread has joined
		if ( pthread_join(pxl->tid, 0) == EOK )
		{
   			 delete pxl;
		}
		else
		{
			std::cout << "TRACE(laser.cpp): laser data thread did not join properly in Laser destructor" << std::endl;
		}
   }
   else
   {   
   		std::cout << "TRACE(laser.cpp): laser data thread did not cancel properly in Laser destructor" << std::endl;
   }

   if( serial_ ) delete serial_;
}

//IniLaserInstance(3,9600,38400,1,11)
// run laser#3, initial speed=9600bps, run speed=38Kbs, init sensor, serial /sev/ser11

// initialise the laser
void* 
Laser::IniLaserInstance(unsigned int n, int speed0, int speed2, int init, int serialPort)
{
    // pointer to the struct which stores the laser params and data
	pxl = new LaserData;

	pxl->sp =  &FrameLsr1;
	pxl->NameUnit = "Laser_1";
	pxl->NameCB = "LSR1";
	pxl->code = 0;
	pxl->sufixFile = "lsr1.dat";
	pxl->MaxFileSize = 0;
	pxl->rr_lsr = NULL;

	// flag which is checked by thread indicating whether it should stop
    pxl->FlagBye=1;
    
	// get the priority of this thread
    pxl->priority2 = getprio( 0 );
	// use this in getprio() for dynamically increasing the priority of
	// this thread by 4
    pxl->priority =  pxl->priority2+4;
    
	// initial baudrate
	pxl->speed = speed0 ;
    // desired baudrate
	pxl->speedB = speed2 ;
    
	pxl->init = init ;
    pxl->PublishFlag = 0xFF ;
        
    sprintf(pxl->NamePort,"/dev/ser%d",serialPort) ;
    
    IniLSRXCode(&pxl->FlagBye,pxl) ;
        
    printf("**init [%s]ser={[%s][%d]}p={[%d][%d]}speed=[%d][%d]\n",pxl->NameUnit,pxl->NamePort,pxl->FComLsr,pxl->priority,pxl->priority2,pxl->speed,pxl->speedB) ;
    return(pxl) ;
}

// use this to cast a function ptr c-style
static void* castFnPtr(Laser *pl)
{
	pl->readSickLaserX(pl->pxl) ;
   return(NULL) ;
}


int 
Laser::IniLSRXCode(int *pflag, struct LaserData *pxl)
{
	int a1=-1,stc=-1;
	char *str ;
	 
	pxl->ThreadData.pflag = pflag;

	pxl->hLHistoric=-1 ;
	pxl->cx=0 ;
	pxl->CxReset=pxl->CxReset2=0;

		stc = serial_->open( pxl->NamePort, 0 ) ;
 
		if (stc<=0)
		{
			printf("FAIL: serial not valid");
			goto chau ;
		}
		
		//a1 = pthread_create( &(pxl->tid), NULL, (FunciThread)readSickLaserX, pxl ) ;
   	    // hack to deal with casting a function pointer in C++
		// TODO: cast this properly using c++ casts
		FunciThread pf ; 
   	    pf = (FunciThread)castFnPtr; 
   	    a1 = pthread_create( &(pxl->tid), NULL, pf, this ) ;
	
	// }
		
	if (a1<0)
    {
        str = "thread [%s] failed\n";
    }
	else
    {
        str = "thread [%s] OK\n";
    }
	if (a1<0)
    {
        goto chau;
    }
	pxl->used=1;
	
	
	return(0);

chau:
	
	pxl->used=0 ;

	//  if (pxl->FComLsr!=0)
	// {
    //     serial_->CloseSerialPortJEG();
    // }

	return(1) ;
} 


#define POLYCRC 0x8005
// static uint16b ExeCRC(unsigned char *pf,int n,uint16b crc_poly);
extern xuint32 ExternalCounter1;

// parse the incoming laser data and put into a buffer for publishing
void 
Laser::parse_SICK_LASER(struct sic_packet *pxxx,unsigned char *string,struct reco *rr_lsr, orca::Time timeStamp, int *BadCrcs,struct LaserData *pxl)
{
    unsigned short *p0 = (unsigned short *)(string+7) ;
	unsigned i;
	uint16b crc1,crc2 ; 
	//struct sic_packet xxx;
	// double dt ;
	crc2 = ExeCRC(string,730,POLYCRC);
	crc1 = *((uint16b*)(string+730)) ;
	
    if (crc1!=crc2)
    {
        (*BadCrcs)++;
    }
	else
	{
        pxxx->timeStamp = timeStamp;
		pxl->cx++ ;
		for (i=0;i<RANGESIZE;i++){	pxxx->range[i]=p0[i] ; }
		pxxx->Status = string[729] ;
		pxxx->cx = pxl->cx ;
        //
        // push the laser data into a buffer for the laser2d component to push to icestorm
        //
        laserDataBuffer_.push( *pxl );

        // dt = orcaice::timeAsDouble(timeStamp) - pxl->LastTxTime;
        //to avoid sending to net at high freq (75hz...)      
		// if (dt>=pxl->DtMinTx)			
		// {	
		    // printf("--->timestamp = %d\n",pxl->sp->timestamp ) ;

			/* HERE IS THE LASER FRAME --> DO SOMETHING with IT.

				"PushLHLaserFrame(pxl);"
				published the data in the on-line DataBAse. It is a fast and non-blocking call.


				You should do something similar.
				DO NOT PRINTF or call any other slow or blocking function here, it could 
				compromise the real time performance of the driver
		
			*/

		   // pxl->LastTxTime = orcaice::timeAsDouble(timeStamp);
		// }
		// else
        // {
        //     pxl->IgnoredFramesB++;
        // }
	}
}


#define STR1	sizeof(string1)
#define STR2	sizeof(string2x)
#define STR3	sizeof(string3)
#define STR4	sizeof(string4x)
#define STR5	sizeof(string5)
#define STRSZ	750

void 
Laser::PutCRCinStr(char *str,int size)
{
	uint16b crc ;
	unsigned char *su=(unsigned char*)str ;
	crc = ExeCRC(su,size,POLYCRC) ;
	su=su+size ;
	su[1]=(crc&0xff00)>>8;
	su[0]=(crc&0x00ff);
}

void 
Laser::readResponseInfo1(int FComLsr)
{	
	char s[220] ;
	static char str[1000] ;
    int n,i,m=0,n0 ;
	
	serial_->timeout( 1, 0 );
	// n = serial_->read_full( (unsigned char*)s, 200, T5segs/10 );
	n = serial_->read_full( (unsigned char*)s, 200 ); 
	if (n>6)
	{	
		n=6 ; 
	}
	  
 	else if( n<0 )
	{
 		 std::cout << "ERROR(readResponseInfo1::laser.cpp): could not read laser's response" << std::endl;    	
	}
    
	for (i=0;i<n;i++)
	{	
		m=m+sprintf(str+m,"|%x|",0x00FF&(unsigned)s[i]) ; 
	}
	
	n0=n;
	str[m]='*' ; str[m+1]=0 ; 
	printf("**-->[%d]:%s,  [%s]\n",n0,str,s+6) ;

}


void 
Laser::readResponseB(int FComLsr,struct LaserData *pg)
{	
	char s[1200] ;
    static char str[1000] ;
	int n,i,m=0,n0 ;
 	serial_->timeout( 1, 0 );
	n = serial_->read_full( (unsigned char *)s, 1000 );
	if (n>10)
	{	
		n=10 ; 
	}
	else if( n<0 )
	{
 		 std::cout << "ERROR(readResponseB::laser.cpp): could not read laser's response" << std::endl;    	
	}
	
	n0=n ;
	
	for (i=0;i<n;i++)
	{
		m=m+sprintf(str+m,"%x|",0x00FF&(unsigned)s[i]) ; 
	}

	str[m]='@' ; str[m+1]=0 ; 
}

void 
Laser::readResponseC(int FComLsr,struct LaserData *pg)
{
	readResponseB(FComLsr,pg);
}

void 
Laser::readResponse(int FComLsr)
{
}



void 
Laser::SetLaser(int FComLsr,struct LaserData *pg)
{
	
	static char string1[]
		= {'\x2','\x0','\xa','\x0','\x20','\x0','S','I','C','K','_','L','M','S','\xbe','\xc5'};
	// change laser operating mode to 'install' mode
	
	//static char string2[] = {'\x2','\x0','\x2','\x0','\x20','\x40','\x50','\x8'};
	static char string2x[] = {'\x2','\x0','\x2','\x0','\x20','\x40',0,0};
	// change baud rAte to 38400
	//string2[5] = 0x40 means 38400b/s.   string2[5] = ????  500Kb/s

	//static char string3[] = {'\x2','\x0','\x2','\x0','\x66','\x1','\x9d','\x4e'};
	// the configured baud rate remains after PowerON
	static char string4x[]
		//= {'\x2','\x0','\x21','\x0','\x77','\x0','\x0','\x0','\x04','\x0',
		= {'\x2','\x0','\x21','\x0','\x77',  0,0,  0,0,  0,	1,   0,
		'\x0','\x0',2, '\x0','\x0','\x0','\x0','\x0','\x0',
		'\x0','\x0','\x0','\x0','\x0','\x0','\x0','\x0','\x0','\x0','\x0',
		'\x0','\x0','\x0','\x0','\x0','\x40','\xab'};
		//'\x0','\x0','\x0','\x0','\x0','\x40','\xab', 0 };
	
	//string4[14] = 2   : average 2 frames..???

	// laser configuration string
	static char string5[]
		= {'\x2','\x0','\x2','\x0',  '\x20','\x24',  '\x34','\x08'};
	// set mode to send continuously
	//static unsigned XXX[] = { sizeof(string1),sizeof(string2),sizeof(string3),sizeof(string4),sizeof(string5) } ;

	//180 deg, 0.5 deg res.
	//static char stringScanDef[] = { 2,0,5,0,  0x3b, 180,0, 50,0,  0,0} ;

	//request type..
	static char stringAskInfo1[] = { 2,0,1,0,  0x3a,  0,0} ;

	char string4b[200] ;
	char string2b[200] ;

	memcpy(string4b,string4x,1+sizeof(string4x)) ;
	memcpy(string2b,string2x,1+sizeof(string2x)) ;

	string2b[5]=0x40 ;
	if (pg->speedB>100000){	string2b[5]=0x48;	}//in 500Kb/s, special hardware
	string4b[14] = 1  ;
	
	//this byte sets the sensitivity
	string4b[11] = (xuint8)LaserSensibilityMode ;

	// set to 'install' mode
	PutCRCinStr(string1,sizeof(string1)-2);
	PutCRCinStr(string2b,sizeof(string2x)-2);
	PutCRCinStr(string4b,sizeof(string4x)-2);
	PutCRCinStr(string5,sizeof(string5)-2);
	PutCRCinStr(stringAskInfo1,sizeof(stringAskInfo1)-2);


	printf("**[%s]:laser mode = [%x],sensitivity=[%d]\n",pg->NameUnit,(int)string2b[5],(int)string4b[11]) ;

	// ---------- at speed  pg->speed (usually 9600)
	////PrintiEtc1("--- [%s]: being configured------ ",pg->NameUnit);	
	
	//fflush(FComLsr);
	if( serial_->baud( pg->speed ) < 0 )
	{
		 std::cout << "ERROR(laser.cpp): baudrate could not be set" << std::endl;
	}

	////PrintiEtc2("[%s]: serial at [%d]",pg->NameUnit,pg->speed);

	// set to 'install' mode
	// serial_->SerialWriteJEG( (unsigned char*)string1, STR1, &hkswrite );
	serial_->write( (unsigned char*)string1, STR1 );
	  
	////PrintiEtc1("[%s]: Sent init string1",pg->NameUnit);
	if (pg->speedB>100)
    {
        readResponseC(FComLsr,pg);
    }
    else
    {
        readResponse(FComLsr);
    }
	
	delay(100);

	// set baud rate to 38400
	// serial_->SerialWriteJEG( (unsigned char*)string2b, STR2, &hkswrite);
    serial_->write( (unsigned char*)string2b, STR2 );

	////PrintiEtc2("[%s]: Sent init string2b, s[5]=[%x]",pg->NameUnit,(int)string2b[5]);
	if (pg->speedB>100)
    {
        readResponseC(FComLsr,pg);
    }
    else
    {
        readResponse(FComLsr);
    }

    delay(100);

	// ---------- at speed  pg->speed (usually 38400 or 115200b (->500000))
	// change the port speed  to higher
	if ( serial_->baud( pg->speedB ) < 0 )
 	{
		 std::cout << "ERROR(laser.cpp): operating baudrate could not be set" << std::endl;
	}
    ////PrintiEtc2("[%s]: serial at [%d]baud",pg->NameUnit,pg->speedB);

	// new
	serial_->write( (unsigned char*)string1, STR1 );
	
	////PrintiEtc1("[%s]: send init string1",pg->NameUnit);
	if (pg->speedB>100)
    {
        readResponseC(FComLsr,pg);
    }
    else
    {
        readResponse(FComLsr);
    }
	
    delay(100);
	//.............

	// more initialisation
	// laser configuration string
	serial_->write( (unsigned char*)string4b, STR4 );
	
	////PrintiEtc1("[%s]: Sent init string4**",pg->NameUnit);
	if (pg->speedB>100)
    {
        readResponseC(FComLsr,pg);
    }
    else
    {
        readResponse(FComLsr);
    }

    delay(100);

	serial_->write( (unsigned char*)stringAskInfo1, sizeof(stringAskInfo1) );

	readResponseInfo1(FComLsr);

	// set mode to send data continuously
	serial_->write( (unsigned char*)string5, STR5 );

	////PrintiEtc1("[%s]: Sent init string5**",pg->NameUnit);
	if (pg->speedB>100)
    {
        readResponseC(FComLsr,pg);
    }
    else
    {
        readResponse(FComLsr);
    }
	
    delay(100);
    
}

void 
Laser::readSickLaser(struct LaserData *pxl,int *pFlag)
{
	int ignoredx=0,ignoredT=0,nnn;
	int synch = 0,xx;
	char string[STRSZ];
	int timestamp=0 ;
	int FComLsr = pxl->FComLsr;
	struct reco *rr_lsr= pxl->rr_lsr;
	int t00 ;
	int cxFailing=0,cxResets=0 ;
	
	pxl->active=1 ;

    if (pxl->init){ SetLaser(FComLsr,pxl); }

	t00=-10000 ;
	// endless loop waiting for data to read
	while(*pFlag)
	{
		// 27/2/2001 - trev. mod. Addition of Watchdog timer
		if ( (timestamp-t00) >10000 )
		{
            t00 =timestamp ;
			//PrintiEtcXX5("**(%s) *[%3d][%5d][f=%3d]*[%3d]\n",pxl->NameUnit,pxl->usedChars,(int)pxl->TimeOuts,(int)pxl->FramesOK,(int)timestamp);
			//PushMsg(hline,1,line,strAuxi) ;
		}

		//here i should increase the priority
		setprio(0,pxl->priority) ;

        // check for incoming header character
        // TODO: get rid of the fileDescriptor method
		xx = orcaserial::waitForByte( '\x02', T5segs, serial_->fileDescriptor() ); // 5 segs timeout

        // increment the no. of bytes ignored (this should be 0 after the first frame)
		ignoredT+= ignoredx ;

        // timestamp the incoming char just in case it was a header character
		// timestamp = (int)GetTimeUTE1();  // <--------- your clock function
		orca::Time timeStamp = orcaice::getNow();
        // std::cout << "orcaTimeStamp: " << orcaice::toString(orcaTimeStamp) << std::endl;

		//here i should decrease the priority
		//setprio(0,pxl->priority2) ;
		if (xx >=0)
		{
	        // if the incoming byte was a header character

			//*timestamp = GetTime();
			pxl->timeStamp=timestamp ;

			//hkSleep(1) ;
			string[0] = '\x02' ; // it is not important, it is only to have it.
            // check the next 3 bytes to confirm that the incoming bytes are really part of a laser frame
			serial_->timeout( 5, 0 );
			nnn = serial_->read_full( (unsigned char*)string+1, 3 );

			// if next character in packet = 0x80
			if ( (nnn==3) && (string[1] == '\x80'))
			{
                // if these bytes were part of the header continue reading

				if (synch==0)
				{
                    synch = 1; // then synchronised!
				}
				
				// continue reading the rest of the laser frame
				serial_->timeout( 5, 0 );
                nnn = serial_->read_full( (unsigned char*)string+4, 728 );

				//if ( (errorno != 728) ||((errorno == 728) && (string[731] != 0x02)))
				if  (nnn != 728)
				{
                    synch = 0;	// we've lost synchronisation
					pxl->ignoredT+=nnn  ;
				}
				else
				{
                    //ChecCRC(string
					//parse_SICK_LASER((unsigned char*)string,rr_lsr,*timestamp,&BadCrcs);
					parse_SICK_LASER( pxl->sp, (unsigned char*)string, rr_lsr,timeStamp, &(pxl->BadCrcs), pxl );
					pxl->usedChars+=732 ;
					pxl->FramesOK++ ;
					//printf("--------- laser\n") ;
				}
			}
			else
			{
                pxl->ignoredT+=nnn  ;pxl->TimeOuts++ ; }
			}
			else
			{
                delay(1);
                pxl->TimeOuts++ ;
                cxFailing++ ;
				if (cxFailing>5)
				{
                    //int stc ;
					if (cxResets>10)
					{
                        //PrintiEtc1("[%s]: too many resets, bye\n",pxl->NameUnit) ;
						goto chau ;
					}
					cxFailing=0 ; cxResets++ ;
					//PrintiEtc3("[%s]: Reset laser, touts=[%d],resets=[%d]\n",pxl->NameUnit,(int)pxl->TimeOuts,cxResets) ;

					/*
					CloseSerialPort(&(pxl->FComLsr)) ;
					stc = OpenSerialPortX(pxl->NamePort,pxl->speed,&(pxl->FComLsr)) ;
					if (stc<0)
					{	PrintiEtc2("[%s]: Can not reopen serial [%s]\n",pxl->NameUnit,pxl->NamePort) ;
						goto chau ;
					}
					PrintiEtc2("[%s]: reopen serial [%s] Ok\n",pxl->NameUnit,pxl->NamePort) ;
					*/
	
					SetLaser(FComLsr,pxl) ;
				}
			}
	}

chau:
    // serial_->CloseSerialPortJEG();
	
	pxl->active=0 ;
	////PrintiEtc1("LaserData [%s] ends\n",pxl->NameUnit);
	printf("LaserData [%s] ends\n",pxl->NameUnit);
}


void* 
Laser::readSickLaserX(void *pxl0)
{
	struct LaserData *pxl = (struct LaserData*)pxl0 ;
 	struct threadData *p = &(pxl->ThreadData) ;
	setprio(0,pxl->priority2) ;
	////PrintiEtcXX2("-> [%s] [prio=%d]\n",pxl->NameUnit,getprio( 0 ));	
	readSickLaser(pxl,p->pflag) ;
   	return(NULL) ;
}



/* -----------------------------------------------------------------
LaserData data frame :

(0)(1)   (2)	       (7)							   (728)	(730)(731)
[2][h80][L2:L1][a,b,c][ 361*2 bytes of data ...............][n][CRC2:CRC1]

then we replaced the integer fields by short integers
in the SIC_PACKET, it is more efficient
jose (3/2000)

2) The CRC function is used.  
-------------------------------------------------------------
*/
/* CRC checking function, Jose, 3\2000) */
uint16b 
Laser::ExeCRC(unsigned char *pf,int n,uint16b crc_poly)
{
   int i  ;
   unsigned char abData[2] ;
   uint16b crc=0 ;

   abData[0]=0 ;
   for (i=0;i<n;i++)
   {
     abData[1] = abData[0] ;
     abData[0]=*pf ; pf++ ;
     if (crc&0x8000)
     {
         crc<<=1 ;  crc^=crc_poly ;
     }
     else
     {
         crc<<=1 ;
     }
     crc^=*((uint16b*)(abData)) ;
   }
   
   return(crc);
}

void 
Laser::ReadRangeData2_U16(uint16b *pf)
{
    int i ;
 	for (i=0;i<RANGESIZE;i++)
    {
        pf[i] = (FrameLsr1.range[i]&0x3FFF) ;
    }
}

// EXAMPLE


//Laser_2 active=[0],priority=[1],com=[4],speed=[9600,38400],init=[1],publish=[1],dtTxMin=[10]
//Laser_2 active=[0],priority=[1],com=[4],speed=[9600,500000],init=[1],publish=[1],dtTxMin=[10]

//2215,115200
// opens two instances of a laser
// main()
// {

	//IniLaserInstance(1,9600,57600,1, 1); //laser4, 500Kbps , /dev/ser14
// 	 IniLaserInstance(2,9600,38400,1, 1);   //lsr2  , 38Kbps  , /dev/ser4
//     sleep(100) ;
// 
// }

