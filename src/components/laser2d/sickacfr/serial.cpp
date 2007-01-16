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

namespace laser2d {

Serial::Serial()
{
}

Serial::~Serial()
{
}

char* 
Serial::SpecifySerialPortString(unsigned np)
{	static char *strs[]=
        { "/dev/ser0",
	  "/dev/ser1","/dev/ser2","/dev/ser3","/dev/ser4","/dev/ser5",
      "/dev/ser6","/dev/ser7","/dev/ser8","/dev/ser9","/dev/ser10",
      "/dev/ser11","/dev/ser12","/dev/ser13","/dev/ser14","/dev/ser15" } ;
	if (np>15){	np=0 ; }
	return(strs[np]) ;
}

int 
Serial::SerialWriteJEG( int port_fd, uchar *buff, int L, int *nw)
{	
	ssize_t  n ;
	
	n = write( port_fd, buff, L);	
	*nw = n ;
	return(n) ;
}

void 
Serial::CloseSerialPortJEG( int* port_fd ) 
{	
	if (port_fd >= 0)
	{	
		close( *port_fd ) ; 
	}
}

int 
Serial::openSerial(char* device, int baud, int* port_fd)
{
    // TODO: move port_fd to a private member when this is a c++ class
	*port_fd = -1;
	int u1,u2;
	struct termios ser_opts;

	*port_fd = modem_open( device, baud ) ;
	if( *port_fd < 0)
	{  
        printf("ERROR(serial.c): Could not open serial device\n");
		return 0; 
	} 

	u1 = tcgetattr( *port_fd, &ser_opts ) ;
	ser_opts.c_cflag = CS8|CREAD|HUPCL|CLOCAL ;  //|IHFLOW 
	ser_opts.c_lflag =IEXTEN ;
	ser_opts.c_oflag =0 ;
	ser_opts.c_iflag = IGNBRK |  IGNPAR ;  //IGNCR |

	u2 = tcsetattr( *port_fd, TCSANOW, &ser_opts ) ;

	return 1 ;

}


int 
Serial::SerialReadJEG(int port_fd, uchar *buff, int lMin, int lMax, int TimeOut)
{	
	int n;
	
	n = readcond(port_fd, buff, lMax, lMin, TimeOut, TimeOut);
	
	return(n) ;
}



int	
Serial::ChangeSerialSpeedJEG(int port_fd, int speed) 	
{
	int x ;
	unsigned speed2 ;
	struct termios termio_p ;
	
	speed2 = (unsigned)((double)speed) ;
	
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


int 
Serial::CharsWaitingJEG( int port_fd )
{	
	return(tcischars(port_fd));	
}


// ================================
//static uchar bufAuxi[200]; static int na=0 ;
//int SerialWaitForCharX(int h, uchar b ,int timeoutDs,int *ignoredChars )
//	x=tcischars(f_id);

// -------------------------------------------------------------------------       				
int 
Serial::SerialReadUntilCharXJEG( int port_fd, uchar *buff, int Lmax,uchar b, int timeoutDs, int *ignoredChars)
{
	struct termios termio;
	int c ;
	int n,x,z ;
	
	x=tcgetattr( port_fd, &termio );	
	
	if (x<0)
	{	
		return -1; 
	}
	
	c = termio.c_cc[VFWD];
	termio.c_cc[VFWD] = b;
	x=tcsetattr( port_fd, TCSANOW, &termio );	
	if (x<0)
	{	
		return(-1) ; 
	}

dale:
	n=readcond(port_fd, buff, Lmax, Lmax, timeoutDs, timeoutDs);

	if (n<1)
	{	
		x=-1 ; 
		goto chau ;  
	}
	
	if (buff[n-1]==b)
	{	
		x=n ; 
		goto chau ;  
	}
	
	if (n==Lmax)
	{  
		goto dale ; 
	}
	x=-1 ;
	
chau:
		termio.c_cc[VFWD]=0 ;						// return the original seting
		z=tcsetattr( port_fd, TCSANOW, &termio );
		
		return(x) ;			
}


int 
Serial::SerialWaitForCharXJEG( int port_fd, uchar b ,int timeoutDs,int *ignoredChars )
{
	struct termios termio;
	int c,ichrs=0 ;
	int n,x,z ;
	int t0,t1,dt0 ; 
	static uchar buff[210]; //garbage, use static buffer
	
	t0 = GetTimeUTE1ms();
	dt0 = timeoutDs*100 ;
	x=tcgetattr( port_fd, &termio );
	if (x<0)
	{	
		return(-1);
	}

	c = termio.c_cc[VFWD];
	termio.c_cc[VFWD] = b;
	x=tcsetattr( port_fd, TCSANOW, &termio );	
	if (x<0)
	{	
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





void 
Serial::InitSerialLib(int *pflagBye)
{
	static int flagIni=0 ;
	if (flagIni){	return ; }
	flagIni=1 ;

}

} // namespace

