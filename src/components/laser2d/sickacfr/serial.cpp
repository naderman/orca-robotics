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

int 
Serial::SerialWriteJEG( uchar *buff, int L, int *nw)
{	
	ssize_t  n ;
	
	n = write( port_fd, buff, L);	
	*nw = n ;
	return(n) ;
}

void 
Serial::CloseSerialPortJEG() 
{	
	if (port_fd >= 0)
	{	
		close( port_fd ) ; 
	}
}

int 
Serial::openSerial(char* device )
{
    // TODO: move port_fd to a private member when this is a c++ class
	port_fd = -1;
	int ret;
	int baud = 9600;

	port_fd = modem_open( device, baud ) ;
	if( port_fd < 0)
	{  
        printf("ERROR(serial.c): Could not open serial device\n");
		return 0; 
	} 

	ret = tcgetattr( port_fd, &ser_opts ) ;
	
	ser_opts.c_cflag = CS8|CREAD|HUPCL|CLOCAL ;  //|IHFLOW 
	ser_opts.c_lflag =IEXTEN ;
	ser_opts.c_oflag =0 ;
	ser_opts.c_iflag = IGNBRK |  IGNPAR ;  //IGNCR |

	ret = tcsetattr( port_fd, TCSANOW, &ser_opts ) ;

	return 1 ;

}


int 
Serial::SerialReadJEG( uchar *buff, size_t count, int timeOut)
{	
	int n = readcond(port_fd, buff, count, count, timeOut, timeOut);
	
	return(n) ;
}



int	
Serial::ChangeSerialSpeedJEG( int baud ) 	
{
	int ret;
	int c;

	ret = cfsetispeed( &ser_opts, (speed_t) baud );
	if ( ret<0 )
	{	
		return(-1) ; 
	}
	
	ret = cfsetospeed( &ser_opts, (speed_t) baud );
	if ( ret<0 )
	{	
		return(-1) ; 
	}
	
    ret = tcsetattr( port_fd, TCSANOW, &ser_opts ) ; 

    tcgetattr( port_fd, &ser_opts ) ;
 
 	c = (int)cfgetospeed( &ser_opts ) ;
   
	return(ret) ;
}

int 
Serial::CharsWaitingJEG()
{	
	return(tcischars(port_fd));	
}

int 
Serial::SerialReadUntilCharXJEG( uchar *buff, int Lmax,uchar b, int timeoutDs, int *ignoredChars)
{
	int c ;
	int ret;
	int n,z ;
	
	c = ser_opts.c_cc[VFWD];
	ser_opts.c_cc[VFWD] = b;
	ret = tcsetattr( port_fd, TCSANOW, &ser_opts );	
	if (ret<0)
	{	
		return(-1) ; 
	}

dale:
	n = readcond(port_fd, buff, Lmax, Lmax, timeoutDs, timeoutDs);

	if ( n<1 )
	{	
		ret = -1 ; 
		goto chau ;  
	}
	
	if ( buff[n-1]==b )
	{	
		ret = n ; 
		goto chau ;  
	}
	
	if ( n==Lmax )
	{  
		goto dale ; 
	}
	ret = -1;
	
chau:
		ser_opts.c_cc[VFWD] = 0 ;						// return the original seting
		z = tcsetattr( port_fd, TCSANOW, &ser_opts );
		
		return(ret);			
}


int 
Serial::SerialWaitForCharXJEG( uchar b ,int timeoutDs,int *ignoredChars )
{
	int c,ichrs=0 ;
	int ret;
	int n,z ;
	int t0,t1,dt0 ; 
	static uchar buff[210]; //garbage, use static buffer
	
	t0 = GetTimeUTE1ms();
	dt0 = timeoutDs*100 ;

	c = ser_opts.c_cc[VFWD];
	ser_opts.c_cc[VFWD] = b;
	ret = tcsetattr( port_fd, TCSANOW, &ser_opts );	
	if ( ret<0 )
	{	
		return(-1) ; 
	}
	
dale:
	//PrintiInt("SerialWaitForCharX [%d]\n",h) ;
	//a[0]=b ; 	Printi(a) ; 	
	n = readcond( port_fd, buff, 200, 200, timeoutDs-1, timeoutDs);
	//Printi("SerialWaitForCharX B\n") ; 	
	if ( n<1 )
	{	
		//Printi("SerialWaitForCharX 0\n") ; 	
		ichrs = 0; 
		ret = -1; 
		goto chau ;  
	}
	if ( buff[n-1]==b )
	{	
		ichrs += n; 
		ret = 1; 
		//Printi("SerialWaitForCharX 1\n") ; 	
		// pss->cx[2]++ ;
		// pss->ign[0]+=(n-1) ;
		goto chau ;  
	}
	if ( n==200 )
	{  
		ichrs += n; 
	  	t1 = GetTimeUTE1ms() ;
	   	
		if ( (t1-t0)>dt0 )
	   	{   //Printi("SerialWaitForCharX 2\n") ; 			
		   	ret = -1; 
			goto chau; 
		}	
	   	//Printi("SerialWaitForCharX 3\n") ; 	
	   	goto dale;
	}
	
	//Printi("SerialWaitForCharX 4\n") ; 	
	ret = -1;

chau:
		//tcgetattr( h, &termio );
		//termio.c_cc[VFWD]=_PC_VDISABLE ;			// return the original seting
		ser_opts.c_cc[VFWD] = 0;						// return the original seting
		z=tcsetattr( port_fd, TCSANOW, &ser_opts );
		*ignoredChars = ichrs;
		return(ret);			

}


} // namespace

