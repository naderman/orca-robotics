/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>    // read and write to ports
#include <string.h>
#include <iostream>
#include "serial.h"

#include <config.h>
#if HAVE_FILIO_H
   // FIONREAD
#  include <sys/filio.h>
#endif
// Ensure we have strnlen
#include <orcaportability/strnlen.h>

#ifdef __QNX__
#include <sys/modem.h>
#include <time.h>
#endif

using namespace std;

namespace orcaserial {

Serial::Serial()
{
    port_fd=-1;
    to_sec=0;
    to_usec=0;
}

Serial::~Serial()
{
    if(port_fd!=-1)
    {
        if(tcdrain(port_fd)){
            perror("Serial::drain():tcdrain()");
            lastError_="Serial::drain():tcdrain()";
        }
        ::close(port_fd);
    }
}

#ifdef __linux

int 
Serial::baud(int baud)
{
    if(port_fd==-1){
        fprintf(stderr,"Serial:baud() no valid device open\n");
        lastError_ = "Serial:baud() no valid device open\n";
        return -1;
    }
    if(tcgetattr(port_fd, &ser_opts) == -1)
    {
        fprintf(stderr,"Serial::baud():tcgetattr() Error reading attr.\n");
        lastError_ = "Serial:baud() no valid device open\n";
    }

    switch(baud){
    case 0:
        cfsetispeed(&ser_opts, B0);
        cfsetospeed(&ser_opts, B0);
        break;
    case 50:
        cfsetispeed(&ser_opts, B50);
        cfsetospeed(&ser_opts, B50);
        break;
    case 75:
        cfsetispeed(&ser_opts, B75);
        cfsetospeed(&ser_opts, B75);
        break;
    case 110:
        cfsetispeed(&ser_opts, B110);
        cfsetospeed(&ser_opts, B110);
        break;
    case 134:
        cfsetispeed(&ser_opts, B134);
        cfsetospeed(&ser_opts, B134);
        break;
    case 150:
        cfsetispeed(&ser_opts, B150);
        cfsetospeed(&ser_opts, B150);
        break;
    case 300:
        cfsetispeed(&ser_opts, B300);
        cfsetospeed(&ser_opts, B300);
        break;
    case 600:
        cfsetispeed(&ser_opts, B600);
        cfsetospeed(&ser_opts, B600);
        break;
    case 1200:
        cfsetispeed(&ser_opts, B1200);
        cfsetospeed(&ser_opts, B1200);
        break;
    case 2400:
        cfsetispeed(&ser_opts, B2400);
        cfsetospeed(&ser_opts, B2400);
        break;
    case 4800:
        cfsetispeed(&ser_opts, B4800);
        cfsetospeed(&ser_opts, B4800);
        break;
    case 9600:
        cfsetispeed(&ser_opts, B9600);
        cfsetospeed(&ser_opts, B9600);
        break;
    case 19200:
        cfsetispeed(&ser_opts, B19200);
        cfsetospeed(&ser_opts, B19200);
        break;
    case 38400:
        cfsetispeed(&ser_opts, B38400);
        cfsetospeed(&ser_opts, B38400);
        break;
    case 57600:
        cfsetispeed(&ser_opts, B57600);
        cfsetospeed(&ser_opts, B57600);
        break;
    case 115200:
        cfsetispeed(&ser_opts, B115200);
        cfsetospeed(&ser_opts, B115200);
        break;
    case 230400:
        cfsetispeed(&ser_opts, B230400);
        cfsetospeed(&ser_opts, B230400);
        break;
    default:
        fprintf(stderr,"Serial::baud() Invalid baud rate.\n");
        lastError_ = "Serial::baud() Invalid baud rate.";
        return -1;
        break;
    }

    if(tcsetattr(port_fd, TCSAFLUSH, &ser_opts) == -1)
    {
        fprintf(stderr,"Serial::baud():tcsetattr() Error setting attr.\n");
        lastError_ = "Serial::baud():tcsetattr() Error setting attr.";
        return -1;
    }

    return 0;
}

int 
Serial::open(const char *device, const int flags)
{
    if((port_fd = ::open(device, flags|O_RDWR|O_NOCTTY))== -1)
    {
        perror("Serial::open()");
        lastError_ = string("Serial::open()") + strerror(errno);
        return -1;
    }

    if(tcgetattr(port_fd, &ser_opts) == -1)
    {
        fprintf(stderr,"Serial::tcgetattr() Error reading attr.\n");
        lastError_ = "Serial::tcgetattr() Error reading attr.";
        return -1;
    }

    // enable receiver & ignore control lines
    ser_opts.c_cflag |=  (CLOCAL | CREAD) ;

    // set 8 data bits
    ser_opts.c_cflag &= ~CSIZE;
    ser_opts.c_cflag |= CS8;

    // set parity to none with no stop bit
    ser_opts.c_cflag &= ~PARENB;
    ser_opts.c_cflag &= ~CSTOPB;

    // disable hardware flow control
    ser_opts.c_cflag &= ~CRTSCTS;

#if defined(__sun)
    // http://www.sunmanagers.org/pipermail/summaries/2005-October/006871.html
    ser_opts.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    ser_opts.c_oflag &= ~OPOST;
    ser_opts.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    ser_opts.c_cflag &= ~(CSIZE|PARENB);
    ser_opts.c_cflag |= CS8;
#else
    cfmakeraw(&ser_opts);
#endif

    if(tcsetattr(port_fd, TCSAFLUSH, &ser_opts) == -1)
    {
        fprintf(stderr,"Serial::tcsetattr() Error setting attr.\n");
        lastError_ = "Serial::tcsetattr() Error setting attr.";
        return -1;
    }
    return 0;
}

int 
Serial::read(void *buf, size_t count)
{
    int got;
    got = ::read(port_fd, buf, count);
    if ( got < 0 )
        lastError_ = string("Serial::read(): ") + strerror(errno);
    return got;
}

int 
Serial::read_line(void *buf, size_t count, char termchar)
{
    int got = 0;
    char lastchar=0;
    do{
        //not enough room in buffer
        if(got==(int)count-1){
            lastError_ = "Serial::read_line(): Not enough room in buffer.";
            return -1;
        }
        char *offset=(char*)buf+got;
        int ret = ::read(port_fd, offset, 1);
        if(ret>=0){
            got += ret;
            lastchar=((char*)buf)[got-1];
            //((char*)buf)[got]=0;
            //printf("got %d bytes: %s\n",got,(char*)buf);
        }else if(ret==-1&&errno==EAGAIN){  // we must be in non-blocking mode
            fd_set rfds;
            struct timeval tv;
            FD_ZERO(&rfds);
            FD_SET(port_fd, &rfds);
            tv.tv_sec = to_sec;
            tv.tv_usec = to_usec;
            int selval = select(port_fd+1, &rfds, NULL, NULL, &tv);
            if(selval==0){
                //printf("select timed out no data\n");
                return 0;
            }
            if(selval<0){
                //perror("select()");
                lastError_ = string("Serial::read_line(): select(): ")+strerror(errno);
                return -1;
            }
        }
    }while(lastchar!=termchar);

    return got;
}

int 
Serial::read_full(void *buf, size_t count)
{
    int got=0;
    while(got<(int)count){
        char *offset=(char*)buf+got;
        int ret = ::read(port_fd, offset, count-got);
        if(ret>=0){
            got += ret;
        }else if(ret==-1&&errno==EAGAIN){  // we must be in non-blocking mode
            fd_set rfds;
            struct timeval tv;
            FD_ZERO(&rfds);
            FD_SET(port_fd, &rfds);
            tv.tv_sec = to_sec;
            tv.tv_usec = to_usec;
            int selval = select(port_fd+1, &rfds, NULL, NULL, &tv);
            if(selval==0){
                //printf("select timed out no data\n");
                lastError_ = "Serial::read_full(): select timed out: no data.";
                return -1;
            }
            if(selval<0){
                lastError_ = string("Serial::read_full(): select(): ")+strerror(errno);
                //perror("select()");
                return -1;
            }
        }
    }
    return got;
}

int 
Serial::data_avail()
{
    int ret,n_read;
    ret=ioctl(port_fd,FIONREAD,&n_read);

    if(ret==-1){
        perror("Serial:data_avail()");
        lastError_ = string("Serial::data_avail(): ")+strerror(errno);
        return -1;
    }
    return n_read;
}

int 
Serial::data_avail_wait()
{
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(port_fd, &rfds);
    tv.tv_sec = to_sec;
    tv.tv_usec = to_usec;
    int selval = select(port_fd+1, &rfds, NULL, NULL, &tv);
    if(selval==0){
        //printf("select timed out no data\n");
        return 0;
    }
    if(selval<0){
        //perror("select()");
        lastError_ = string("Serial::data_avail_wait(): select(): ")+strerror(errno);
        return -1;
    }
    return data_avail();
}

int 
Serial::write(const void *buf, size_t count)
{
    int put;
    put = ::write(port_fd, buf, count);
    if ( put < 0 )
        lastError_ = string("Serial::write(): ")+strerror(errno);
    return put;
}

int 
Serial::write(const char *str, size_t maxlen)
{
    int toput=strnlen(str, maxlen);
    int put;
    put = ::write(port_fd, str, toput);
    if ( put < 0 )
        lastError_ = string("Serial::write(): ")+strerror(errno);
    return put;
}

int 
Serial::flush()
{
    return(tcflush(port_fd,TCIOFLUSH));
}

int 
Serial::drain()
{
    // wait till all output sent
    if(tcdrain(port_fd)){
        perror("Serial::drain():tcdrain()");
        lastError_ = string("Serial::drain():tcdrain(): ")+strerror(errno);
        return -1;
    }
    else{
        return 0;
    }
}

#endif

#ifdef __QNX__

int 
Serial::baud(int baud)
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

// flags are not used here
int 
Serial::open(const char* device, const int flags)
{
	int ret;
	int baud = 9600;
 
    // argument to modem_open requires a non_const pointer
	port_fd = modem_open( const_cast<char*>(device), baud ) ;
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
Serial::read(void *buf, size_t count)
{
}

int 
Serial::read_line(void *buf, size_t count, char termchar)
{
}

int 
Serial::read_full(void *buf, size_t count)
{
    // these timeouts and elapsedTime do not have to be accurate so can just
	// use ints
	int timeOut = to_sec + to_usec/1000000;
	// cout << "timeout: " << timeOut << endl;

	// total time elapsed while trying to read from serial device
	int elapsedTime;

	// total number of bytes read 
	int got = 0;
    
	// number of bytes read in one readcond() function call 
	int ret = 0;
	
    // maximum number of bytes to read in one readcond() function call
	int max = 15;

	// number of bytes in each chunk that "count" is split into
    int countChunk;
	
    // total bytes left to read
	int bytesLeft;
	
    struct timespec tStart;
    struct timespec tEnd;
 
	clock_gettime( CLOCK_MONOTONIC, &tStart );

	while (got < count)
	{
 	    // offset to the start of the buffer
		char* offset=(char*)buf+got;
    
        // We need to read in small chunks for certain serial managers.
		// This is a little more inefficient than reading all bytes at once 
		
        bytesLeft = count-got;
		
		if ( bytesLeft>max )
		{
		    countChunk = max; 
		}
		else
		{
			countChunk = bytesLeft;
		}
		
		ret = ::readcond(port_fd, (void*)offset, countChunk, countChunk, timeOut, timeOut );
		// cout << "Read " << ret << " bytes out of " << count  << " bytes" << endl;
		got += ret;

 		clock_gettime( CLOCK_MONOTONIC, &tEnd );
        // cout << "TIMEOUT: " << tEnd.tv_sec-tStart.tv_sec << endl;
		elapsedTime = tEnd.tv_sec-tStart.tv_sec + (tEnd.tv_nsec-tStart.tv_nsec)/1000000000; 
		
		if ( elapsedTime > timeOut*5 )
		{
			break;
		}
		
	}
	
	//int n = readcond(port_fd, buf, count, count, timeOut, timeOut);
	// cout << "got: " << got << endl;
	return(got) ;
}

int 
Serial::data_avail()
{	
    int ret,n_read;
    ret = ioctl(port_fd,FIONREAD,&n_read);

    if( ret==-1 )
	{
        perror( "Serial:data_avail()" );
        lastError_ = string("Serial::data_avail(): ")+strerror(errno);
        return -1;
    }
    return n_read;

// return( tcischars(port_fd) );	 
}

int 
Serial::data_avail_wait()
{
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(port_fd, &rfds);
    tv.tv_sec = to_sec;
    tv.tv_usec = to_usec;
    int selval = select(port_fd+1, &rfds, NULL, NULL, &tv);
    if(selval==0){
        //printf("select timed out no data\n");
        return 0;
    }
    if(selval<0){
        //perror("select()");
        lastError_ = string("Serial::data_avail_wait(): select(): ")+strerror(errno);
        return -1;
    }
    return data_avail();
}

int 
Serial::write(const void *buf, size_t count)
{
    ssize_t n;
	
	n = ::write( port_fd, buf, count );	
	// *nw = n ;
	return(n) ;
}

int 
Serial::write(const char *str, size_t maxlen)
{
}

int 
Serial::flush()
{
}

int 
Serial::drain()
{
}


#endif


} // namespace
