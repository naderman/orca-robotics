/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Jose Guivant, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

// SERIALLIB.C === SERIAL PORT FUNCTIONS ====

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/modem.h>
#include <sys/uio.h>
#include <termios.h>
#include <errno.h>
#include "sickdefines.h"
#include "time.h"
#include "serial.h"
//...................................................

struct SerialDefinition
{
    // file descriptor
	int H ;
	// double ClockFactor ;
	// struct serStats *pSE ;
	// int used ;
	// xuint32 marca1 ;
	// int ii ;
};


static struct SerialDefinition* GetNewSerialItem(void)
{
	struct SerialDefinition *p ;
	p = (struct SerialDefinition *)malloc(sizeof(struct SerialDefinition)) ;
	if (p!=NULL)
	{	// p->marca1 = 123456789 ; 
		// p->pSE = NULL ; 
		p->H = -1 ;
		// p->used = 1;
	}
	return(p) ;
}

static void ReleaseSerialItem(struct SerialDefinition*ph)
{	
	// ph->marca1=0 ; 
	// ph->used=0 ; 
	free(ph) ; 
}

//...........................................................
//static xuint8 hx[64] ;
// struct serStats
// { 
// 	int used ;
// 	int cx[4] ;
// 	int ign[5] ;
// 	int err[10] ;
// 	int vfrdDisChar ;
// 	struct SerialDefinition *ph ;
// };

// static struct serStats SSx[32];
// #define GetpSserStats(x)	(SSx+(31&(unsigned)x))

//------------------------------------------------------------------------
// -------------------------------------------------------------------------       				


static int GetSerialPortNumberFromName(char *s)
{	int n=-1 ;
	sscanf(s,"/dev/ser%d",&n) ;
	if (n<0){	return(-1) ; }
	return(n) ;
}



char* SpecifySerialPortString(unsigned np)
{	static char *strs[]=
        { "/dev/ser0",
	  "/dev/ser1","/dev/ser2","/dev/ser3","/dev/ser4","/dev/ser5",
      "/dev/ser6","/dev/ser7","/dev/ser8","/dev/ser9","/dev/ser10",
      "/dev/ser11","/dev/ser12","/dev/ser13","/dev/ser14","/dev/ser15" } ;
	if (np>15){	np=0 ; }
	return(strs[np]) ;
}



//struct SerialDefinition* GetSerialItem(void)
int SerialWriteJEG( int port_fd, uchar *buff, int L, int *nw)
{	ssize_t  n ;
	// struct SerialDefinition* ph = (struct SerialDefinition*)H ;
	// if (ph==NULL){	return(0) ; }
	// if (ph->marca1!=123456789){	return(0) ; }

	
	n = write( port_fd, buff, L);	
	*nw = n ;
	return(n) ;
}
void CloseSerialPortJEG( int* port_fd ) 
{	// struct SerialDefinition** pph = (struct SerialDefinition**)H ;
	// struct SerialDefinition* ph = *pph ;
	// if (ph==NULL){	return ; }
	// if (ph->marca1!=123456789){	return ; }
	if (port_fd >= 0)
	{	
		close( port_fd ) ; 
	}
	// *pph=NULL ;
	// if (ph->pSE!=NULL){	ph->pSE->ph=NULL ; ph->pSE=NULL ; }
	// ReleaseSerialItem(ph) ;

}

static int NumOfOpenedSerials=0 ;

// int OpenSerialPortXJEG(char* device,int speed0, unsigned* H)
int openSerial(char* device, int baud, int* port_fd)
{
    // TODO: move port_fd to a private member when this is a c++ class
	// int port_fd=-1;
	*port_fd = -1;
	int u1,u2;
	struct termios ser_opts;

	// struct SerialDefinition** pph = (struct SerialDefinition**)H;
	// struct SerialDefinition* ph = NULL;

	NumOfOpenedSerials++ ;

	// ph=GetNewSerialItem();
	// ph->marca1=123456789;
	// ph->used = 1 ;

	*port_fd = modem_open( device, baud ) ;
	if( *port_fd < 0)
	{  
        printf("ERROR(serial.c): Could not open serial device\n");
		return 0; 
	} 

	// ph->ii = GetSerialPortNumberFromName(name) ;
	// ph->ClockFactor=1.0 ;

	u1 = tcgetattr( *port_fd, &ser_opts ) ;
	ser_opts.c_cflag = CS8|CREAD|HUPCL|CLOCAL ;  //|IHFLOW 
	ser_opts.c_lflag =IEXTEN ;
	ser_opts.c_oflag =0 ;
	ser_opts.c_iflag = IGNBRK |  IGNPAR ;  //IGNCR |

	u2 = tcsetattr( *port_fd, TCSANOW, &ser_opts ) ;

	// ph->H=port_fd ;
	// {	
		// struct serStats *pss = GetpSserStats(port_fd); pss->used++ ; 
		// if ((u1<0)||(u2<0)){	pss->err[1]=10000; }
		// pss->vfrdDisChar = fpathconf(port_fd, _PC_VDISABLE);
		// if (pss->vfrdDisChar <0){	pss->vfrdDisChar =(long)-errno ; }
		// ph->pSE = pss ;
		// pss->ph = ph ; 
	// }
	// *pph = ph ;
	
	return(1) ;

// chau: *pph=NULL ;
// 	  if (ph!=NULL)
// 	  {	
// 		  ReleaseSerialItem(ph);	
// 	  }
	
// 	  return(0);

}


int SerialReadJEG(int port_fd, uchar *buff, int lMin, int lMax, int TimeOut)
{	// struct SerialDefinition* ph = (struct SerialDefinition*)H ;
	int n;
	// if (ph==NULL){	return(0) ; }					//sleepmilisecs(20)?
	// if (ph->marca1!=123456789){	return(0) ; }		//sleepmilisecs(20)?
	
	n = readcond(port_fd, buff, lMax, lMin, TimeOut, TimeOut);
	{	
		// struct serStats *pss = ph->pSE; 
		// pss->cx[0]+=n ; 
		// if ( n<lMin )
		// {	
		// 	pss->err[8]++ ; 
		// }
	}
	
	return(n) ;
}


int LastSerialSpeedDone ;
// int	ChangeSerialSpeedJEG(unsigned hh,int speed)
int	ChangeSerialSpeedJEG(int port_fd, int speed) 	
{
	int x ;
	unsigned speed2 ;
	struct termios termio_p ;
	// struct SerialDefinition* ph = (struct SerialDefinition*)hh ;
    // if (ph==NULL){	return(0) ; }
	// if (ph->marca1!=123456789){	return(0) ; }

   // speed2 = (unsigned)(ph->ClockFactor*(double)speed) ;
	speed2 = (unsigned)((double)speed) ;
    // printf("ph->H %d \n",ph->H);
	
	// tcgetattr(ph->H,&termio_p) ;
    tcgetattr(port_fd, &termio_p) ; 
	
	x=cfsetispeed(&termio_p,(speed_t) speed2 );
	if (x<0)
	{	
		return(-1) ; 
	}
	
	x=cfsetospeed(&termio_p,(speed_t) speed2 );
	if (x<0)
	{	
		return(-1) ; 
	}
	
	// x=tcsetattr(ph->H,TCSANOW,&termio_p) ;
     x=tcsetattr(port_fd, TCSANOW,&termio_p) ; 

    tcgetattr(port_fd,&termio_p) ;
 
 	LastSerialSpeedDone = (int)cfgetospeed(&termio_p) ;
   
	return(x) ;
}

/*
	example:
				normal serial : ClockFactor = 1.o
				500kb/s serial : ClockFactor = 115200.0/500000.0

Jose, 2005
*/


int CharsWaitingJEG( int port_fd )
{	// struct SerialDefinition* ph = (struct SerialDefinition*)H ;
	// if (ph==NULL){	return(0) ; }
	// if (ph->marca1!=123456789){	return(0) ; }
	return(tcischars(port_fd));	
}


// ================================
//static uchar bufAuxi[200]; static int na=0 ;
//int SerialWaitForCharX(int h, uchar b ,int timeoutDs,int *ignoredChars )
//	x=tcischars(f_id);

// -------------------------------------------------------------------------       				
int SerialReadUntilCharXJEG( int port_fd, uchar *buff, int Lmax,uchar b, int timeoutDs, int *ignoredChars)
{
	struct termios termio;
	int c ;
	int n,x,z ;
	// struct SerialDefinition* ph = (struct SerialDefinition*)H ;
	// struct serStats *pss;
	
	// if (ph==NULL){	return(-1) ; }				//sleepmilisecs(20)?
	// if (ph->marca1!=123456789){	return(-1) ; }	//sleepmilisecs(20)?
	

	// pss=ph->pSE ;

	x=tcgetattr( port_fd, &termio );	
	
	if (x<0)
	{	
		// pss->err[0]++ ;	
		return(-1) ; 
	}
	
	c = termio.c_cc[VFWD];
	termio.c_cc[VFWD] = b;
	x=tcsetattr( port_fd, TCSANOW, &termio );	
	if (x<0)
	{	
		// pss->err[1]++ ; 
		return(-1) ; 
	}

dale:
	n=readcond(port_fd, buff, Lmax, Lmax, timeoutDs, timeoutDs);

	if (n<1)
	{	
		x=-1 ; 
		// pss->err[2]++ ; 
		goto chau ;  
	}
	
	if (buff[n-1]==b)
	{	
		// pss->cx[1]+=n ; 
		x=n ; 
		goto chau ;  
	}
	
	if (n==Lmax)
	{  
		// pss->err[7]++ ; 
		goto dale ; 
	}
	x=-1 ;
	
chau:
		//tcgetattr( h, &termio );
		//termio.c_cc[VFWD]=_PC_VDISABLE ;			// return the original seting
		termio.c_cc[VFWD]=0 ;						// return the original seting
		z=tcsetattr( port_fd, TCSANOW, &termio );
		
		return(x) ;			
}


int SerialWaitForCharXJEG( int port_fd, uchar b ,int timeoutDs,int *ignoredChars )
{
	struct termios termio;
	int c,ichrs=0 ;
	int n,x,z ;
	int t0,t1,dt0 ; 
	static uchar buff[210]; //garbage, use static buffer
	// struct SerialDefinition* ph = (struct SerialDefinition*)H ;
	//struct serStats *pss = GetpSserStats(h);
	// struct serStats *pss ;
	// if (ph==NULL){	return(0) ; }					//sleepmilisecs(20)?
	// if (ph->marca1!=123456789){	return(0) ; }		//sleepmilisecs(20)?

	
	// pss = ph->pSE ;
	
	//char a[] = "  SerialWaitForCharX A\n" ;
	t0 = GetTimeUTE1ms();
	dt0 = timeoutDs*100 ;
	x=tcgetattr( port_fd, &termio );
	if (x<0)
	{	
		// pss->err[3]++ ; 
		return(-1);
	}

	c = termio.c_cc[VFWD];
	termio.c_cc[VFWD] = b;
	x=tcsetattr( port_fd, TCSANOW, &termio );	
	if (x<0)
	{	
		// pss->err[4]++ ;  
		return(-1) ; 
	}
	
dale:
	//PrintiInt("SerialWaitForCharX [%d]\n",h) ;
	//a[0]=b ; 	Printi(a) ; 	
	n=readcond( port_fd, buff, 200, 200, timeoutDs-1, timeoutDs);
	//Printi("SerialWaitForCharX B\n") ; 	
	if (n<1)
	{	
		//Printi("SerialWaitForCharX 0\n") ; 	
		ichrs=0 ; 
		x=-1 ; 
		// pss->err[5]++ ; 
		goto chau ;  
	}
	if (buff[n-1]==b)
	{	
		ichrs+=n ; 
		x=1 ; 
		//Printi("SerialWaitForCharX 1\n") ; 	
		// pss->cx[2]++ ;
		// pss->ign[0]+=(n-1) ;
		goto chau ;  
	}
	if (n==200)
	{  
		ichrs+=n ; 
	  	// t1 = GetXTimeNow();
	  	t1 = GetTimeUTE1ms() ;
	   	
		if ((t1-t0)>dt0)
	   	{   //Printi("SerialWaitForCharX 2\n") ; 			
		   	x=-1 ; 
			// pss->err[6]++ ; 
			goto chau ; 
		}	
	   	//Printi("SerialWaitForCharX 3\n") ; 	
	   	goto dale ;
	}
	
	//Printi("SerialWaitForCharX 4\n") ; 	
	x=-1 ;

chau:
		//tcgetattr( h, &termio );
		//termio.c_cc[VFWD]=_PC_VDISABLE ;			// return the original seting
		termio.c_cc[VFWD]=0 ;						// return the original seting
		z=tcsetattr( port_fd, TCSANOW, &termio );
		*ignoredChars = ichrs ;
		return(x) ;			

}
//--------------------------------------------------





void InitSerialLib(int *pflagBye)
{
	static int flagIni=0 ;
	if (flagIni){	return ; }
	flagIni=1 ;
	//IniClockFactors();
	//RegiPrintInfoFunction(PrintiInfoAboutThisItem,NULL) ;
	//IniTestSerialLibSpeeds(pflagBye);
	//IniModuleVersion();

}

