 /*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifdef __QNX__

// TODO: don't use orca time 

#include <sys/time.h>
#include <termios.h>
#include <unistd.h>

using namespace std;

namespace orcaserial {






//TODO: Duncan, comment. This should be using the doBlocking()
// get rid of port_fd
int
waitForByte( char byte, int timeout, int port_fd )
{
 	int c;
	// int ichrs=0 ;
	int ret;
	int n,z ;
	int t0,t1,dt0 ; 
	static char buff[210]; //garbage, use static buffer
	struct termios ser_opts;

	// t0 = GetTimeUTE1ms();
	struct timeval t;
	void* null = 0;
	gettimeofday( &t, null );
	// convert to ms
	t0 = t.tv_sec * 1000 + t.tv_usec/1000;
	dt0 = timeout*100 ;
	
	ret = tcgetattr( port_fd, &ser_opts );
	c = ser_opts.c_cc[VFWD];
	ser_opts.c_cc[VFWD] = byte;
	ret = tcsetattr( port_fd, TCSANOW, &ser_opts );	
	if ( ret<0 )
	{	
		return(-1) ; 
	}
	
dale:
	//PrintiInt("SerialWaitForCharX [%d]\n",h) ;
	//a[0]=b ; 	Printi(a) ; 	
	n = readcond( port_fd, buff, 200, 200, timeout-1, timeout );
	//Printi("SerialWaitForCharX B\n") ; 	
	if ( n<1 )
	{	
		//Printi("SerialWaitForCharX 0\n") ; 	
		// ichrs = 0; 
		ret = -1; 
		goto chau ;  
	}
	if ( buff[n-1]==byte )
	{	
		// ichrs += n; 
		ret = 1; 
		//Printi("SerialWaitForCharX 1\n") ; 	
		// pss->cx[2]++ ;
		// pss->ign[0]+=(n-1) ;
		goto chau ;  
	}
	if ( n==200 )
	{  
		// ichrs += n; 
	  	
		// t1 = GetTimeUTE1ms() ;
		gettimeofday( &t, null );
		// convert to ms
		t1 = t.tv_sec * 1000 + t.tv_usec/1000;
		
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
		// *ignoredChars = ichrs;
		return(ret);			
 
}

} // namespace

#endif
