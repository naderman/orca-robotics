/* -------------------- LASER.C -------------------------------------- */
// --- Jose Guivant-2000
// modified by Ben Upcroft 2006

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
// #include<sys/neutrino.h>

#include "basics2.h"
#include "timelib.h"
#include "seriallib.h"
#include "laser.h"

namespace laser2d {
   
// #define LASER_TIME_OFF   5000
// #define THREAD_LASER_WAIT 25000
// #define LASER_STACK_SIZE 4096

// read data from the sick laser
static void readSickLaserX(struct Laser *pxl) ;
struct sic_packet FrameLsr1 ;
struct sic_packet FrameLsr2 ;
struct sic_packet FrameLsr3 ;
struct sic_packet FrameLsr4 ;
struct sic_packet FrameLsr5 ;
struct sic_packet FrameLsr6 ;
struct sic_packet FrameLsr7 ;


static struct Laser XLsr1 = {&FrameLsr1,"Laser_1","LSR1",0,"lsr1.dat",0,NULL } ;
static struct Laser XLsr2 = {&FrameLsr2,"Laser_2","LSR2",0,"lsr2.dat",0,NULL } ;
static struct Laser XLsr3 = {&FrameLsr3,"Laser_3","LSR3",0,"lsr3.dat",0,NULL } ;
static struct Laser XLsr4 = {&FrameLsr4,"Laser_4","LSR4",0,"lsr4.dat",0,NULL } ;
static struct Laser XLsr5 = {&FrameLsr5,"Laser_5","LSR5",0,"lsr5.dat",0,NULL } ;
static struct Laser XLsr6 = {&FrameLsr6,"Laser_6","LSR6",0,"lsr5.dat",0,NULL } ;
static struct Laser XLsr7 = {&FrameLsr7,"Laser_7","LSR7",0,"lsr5.dat",0,NULL } ;

static struct Laser *XLsrList[] = { &XLsr1,&XLsr2,&XLsr3,&XLsr4,&XLsr5,&XLsr6,&XLsr7 } ;
#define NumXLsrs (sizeof(XLsrList)/sizeof(XLsrList[0]) )
// --------------------------------------

static uint LaserSensibilityMode=0 ;
// ...........................................................
// #define LENGHCBUFF 50 


//IniLaserInstance(3,9600,38400,1,11)
// run laser#3, initial speed=9600bps, run speed=38Kbs, init sensor, serial /sev/ser11


// initialise the laser
void* IniLaserInstance(unsigned int n, int speed0, int speed2, int init, int serialPort)
{
    struct Laser *pxl ;
    n--;
    if (n>=NumXLsrs)
    {
        return(NULL);
    }
    
    pxl = XLsrList[n] ;
    pxl->FlagBye=1 ;
        
    pxl->priority2 = getprio( 0 ); ;
    pxl->priority =  pxl->priority2+4 ;
    pxl->speed = speed0 ;
    pxl->speedB = speed2 ;
    pxl->init = init ;
    pxl->PublishFlag = 0xFF ;
        
    sprintf(pxl->NamePort,"/dev/ser%d",serialPort) ;
    IniLSRXCode(&pxl->FlagBye,pxl) ;
        
    printf("**init [%s]ser={[%s][%d]}p={[%d][%d]}speed=[%d][%d]\n",pxl->NameUnit,pxl->NamePort,pxl->FComLsr,pxl->priority,pxl->priority2,pxl->speed,pxl->speedB) ;
        
    return(pxl) ;
}

int IniLSRXCode(int *pflag, struct Laser *pxl)
{
	int a1=-1,stc=-1;
	char *str ;
	 
	pxl->ThreadData.pflag = pflag;

	pxl->hLHistoric=-1 ;
	pxl->cx=0 ;
	pxl->CxReset=pxl->CxReset2=0;

	//PrintiNow;
	if (pxl->simu)
	{
        pxl->FComLsr = 0 ;
	}
	else
	{
	    unsigned int uintFComLsr = (unsigned int)(pxl->FComLsr);
		// stc = OpenSerialPortXJEG(pxl->NamePort,pxl->speed, &( pxl->FComLsr ) ) ;

		stc = OpenSerialPortXJEG(pxl->NamePort,pxl->speed, &uintFComLsr ) ;

		if (stc<=0)
		{
            //PrintiEtcXX2("*FAIL (serial not valid) [%s][%s]\n",pxl->NameUnit,pxl->NamePort) ;
			//// <------ yo do not have this service
			goto chau ;
		}
		a1 = pthread_create(&(pxl->tid),NULL,(FunciThread)readSickLaserX,pxl) ;
	}
		
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

	if (pxl->FComLsr!=0)
	{
	    unsigned int uintFComLsr = (unsigned int)(pxl->FComLsr);
        // CloseSerialPortJEG(&(pxl->FComLsr));
		CloseSerialPortJEG( &uintFComLsr );
    }

	return(1) ;
} 


#define POLYCRC 0x8005
static uint16b ExeCRC(unsigned char *pf,int n,uint16b crc_poly);
extern xuint32 ExternalCounter1;

// temp variable to check we are in this method... can remove it later
static int cxx ;

// parse the incoming laser data and put into a buffer for publishing
static void parse_SICK_LASER(struct sic_packet *pxxx,unsigned char *string,struct reco *rr_lsr,xuint32 timestamp,int *BadCrcs,struct Laser *pxl)
{
    unsigned short *p0 = (unsigned short *)(string+7) ;
	unsigned i;
	uint16b crc1,crc2 ; 
	//struct sic_packet xxx;
	xuint32 dt ;
	crc2 = ExeCRC(string,730,POLYCRC);
	crc1 = *((uint16b*)(string+730)) ;
	
	// can remove this later
    cxx++ ;
    printf("--->[%d]\n",cxx) ;
	
    if (crc1!=crc2)
    {
        (*BadCrcs)++;
    }
	else
	{
        pxxx->timestamp = timestamp;
		pxl->cx++ ;
		for (i=0;i<RANGESIZE;i++){	pxxx->range[i]=p0[i] ; }
		pxxx->Status = string[729] ;
		pxxx->cx = pxl->cx ;
        //
        // laser data should be put into buffer for publishing here
        //

		dt = timestamp - pxl->LastTxTime;
        //to avoid sending to net at high freq (75hz...)      
		if (dt>=pxl->DtMinTx)			
		{	
			/* HERE IS THE LASER FRAME --> DO SOMETHING with IT.

				"PushLHLaserFrame(pxl);"
				published the data in the on-line DataBAse. It is a fast and non-blocking call.


				You should do something similar.
				DO NOT PRINTF or call any other slow or blocking function here, it could 
				compromise the real time performance of the driver
		
			*/

			pxl->LastTxTime = timestamp;
		}
		else
        {
            pxl->IgnoredFramesB++;
        }
	}
}


#define STR1	sizeof(string1)
#define STR2	sizeof(string2x)
#define STR3	sizeof(string3)
#define STR4	sizeof(string4x)
#define STR5	sizeof(string5)
#define STRSZ	750

static void PutCRCinStr(char *str,int size)
{
	uint16b crc ;
	unsigned char *su=(unsigned char*)str ;
	crc = ExeCRC(su,size,POLYCRC) ;
	su=su+size ;
	su[1]=(crc&0xff00)>>8;
	su[0]=(crc&0x00ff);
}

static void readResponseInfo1(int FComLsr)
{	
	char s[220] ;
	static char str[1000] ;
    int n,i,m=0,n0 ;
	n=SerialReadJEG( FComLsr,  (unsigned char*)s, 200, 200, T5segs/10 );
	n0=n ;
	if (n>6){	n=6 ; }
	for (i=0;i<n;i++){	m=m+sprintf(str+m,"|%x|",0x00FF&(unsigned)s[i]) ; }
	str[m]='*' ; str[m+1]=0 ; 
	printf("**-->[%d]:%s,  [%s]\n",n0,str,s+6) ;

}


static void readResponseB(int FComLsr,struct Laser *pg)
{	
	char s[1200] ;
    static char str[1000] ;
	int n,i,m=0,n0 ;
	n=SerialReadJEG( FComLsr, (unsigned char*)s, 1000, 1000, T5segs/5 );
	n0=n ;
	if (n>10){	n=10 ; }
	for (i=0;i<n;i++){	m=m+sprintf(str+m,"%x|",0x00FF&(unsigned)s[i]) ; }
	str[m]='@' ; str[m+1]=0 ; 
}

static void readResponseC(int FComLsr,struct Laser *pg)
{
	readResponseB(FComLsr,pg);
}
static void readResponse(int FComLsr)
{
/*
	char s[200] ;
    static char str[1000] ;
	int n,i,m ;
	n=SerialReadJEG(FComLsr,s,100,100,T5segs/10);
	if (n>30){	n=30 ; }
	for (i=0;i<n;i++){	m=m+sprintf(str+m,"[%x]",0x00FF&(unsigned)s[i]) ; }
	sprintf(str+m,"\n");
	Printi(str) ;
*/
}



void SetLaser(int FComLsr,struct Laser *pg)
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

	int hkswrite;
	
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
	ChangeSerialSpeedJEG(FComLsr,pg->speed) ;
	////PrintiEtc2("[%s]: serial at [%d]",pg->NameUnit,pg->speed);

	// set to 'install' mode
	SerialWriteJEG( FComLsr, (unsigned char*)string1, STR1, &hkswrite );
	
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
	SerialWriteJEG( FComLsr, (unsigned char*)string2b, STR2, &hkswrite);

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
	ChangeSerialSpeedJEG(FComLsr,pg->speedB) ;
	////PrintiEtc2("[%s]: serial at [%d]baud",pg->NameUnit,pg->speedB);

	// new
	SerialWriteJEG(FComLsr, (unsigned char*)string1, STR1, &hkswrite);
	
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
	SerialWriteJEG(FComLsr, (unsigned char*)string4b, STR4, &hkswrite);
	
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

	SerialWriteJEG(FComLsr, (unsigned char*)stringAskInfo1, sizeof(stringAskInfo1), &hkswrite);

	readResponseInfo1(FComLsr);
	//readResponseB(FComLsr);


	// set mode to send data continuously
	SerialWriteJEG(FComLsr, (unsigned char*)string5, STR5, &hkswrite);

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

	//SerialFlushInput(FComLsr);
}

static void readSickLaser(struct Laser *pxl,int *pFlag)
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
		xx = SerialWaitForCharXJEG(FComLsr,'\x02' ,T5segs,&ignoredx); // 5 segs timeout

		ignoredT+= ignoredx ;

        // get timestamp (This function does not exist)
        // this uses cpu clock and should be changed to OS clock
		timestamp = (int)GetTimeUTE1();  // <--------- your clock function
		//here i should decrease the priority
		//setprio(0,pxl->priority2) ;
		if (xx >=0)
		{
			//*timestamp = GetTime();
			pxl->timeStamp=timestamp ;

			//hkSleep(1) ;
			string[0] = '\x02' ; // it is not important, it is only to have it.
			nnn = SerialReadJEG(FComLsr, (unsigned char*)string+1, 3, 3, T5segs);

			// if next character in packet = 0x80
			if ( (nnn==3) && (string[1] == '\x80'))
			{
                if (synch==0)
				{
                    synch = 1; // then synchronised!
				}
				
                nnn = SerialReadJEG(FComLsr, (unsigned char*)string+4, 728, 728, T5segs);

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
					parse_SICK_LASER(pxl->sp,(unsigned char*)string,rr_lsr,timestamp,&(pxl->BadCrcs),pxl);
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
                delay(1); ;	pxl->TimeOuts++ ; cxFailing++ ;
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
	////CloseCBHK(&(pxl->rr_lsr));
    unsigned int uintFComLsr = (unsigned int)(pxl->FComLsr);
	//CloseSerialPortJEG(&(pxl->FComLsr)) ;
	CloseSerialPortJEG( &uintFComLsr ) ;
	pxl->active=0 ;
	////PrintiEtc1("Laser [%s] ends\n",pxl->NameUnit);
	printf("Laser [%s] ends\n",pxl->NameUnit);
}


static void readSickLaserX(struct Laser *pxl)
{
    struct threadData *p = &(pxl->ThreadData) ;
	setprio(0,pxl->priority2) ;
	////PrintiEtcXX2("-> [%s] [prio=%d]\n",pxl->NameUnit,getprio( 0 ));	
	readSickLaser(pxl,p->pflag) ;
}


/* -----------------------------------------------------------------
Laser data frame :

(0)(1)   (2)	       (7)							   (728)	(730)(731)
[2][h80][L2:L1][a,b,c][ 361*2 bytes of data ...............][n][CRC2:CRC1]

then we replaced the integer fields by short integers
in the SIC_PACKET, it is more efficient
jose (3/2000)

2) The CRC function is used.  
-------------------------------------------------------------
*/
/* CRC checking function, Jose, 3\2000) */
static uint16b ExeCRC(unsigned char *pf,int n,uint16b crc_poly)
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

void ReadRangeData2_U16(uint16b *pf)
{
    int i ;
 	for (i=0;i<RANGESIZE;i++)
    {
        pf[i] = (FrameLsr1.range[i]&0x3FFF) ;
    }
}

/*static void PushLHLaserFrame(struct Laser *pg)
{
	//struct LaserComp LC;
	if (pg->hLHistoric<0)
    {
        return ;
    }
	//PushRecordInLocalHistoryItem(pg->hLHistoric,pg->sp);
	// YDNHTS  (you do not have this service) 
}

*/
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

} // namespace

#/** PhEDIT attribute block
#-10:100
#**  PhEDIT attribute block ends (-0000084)**/
