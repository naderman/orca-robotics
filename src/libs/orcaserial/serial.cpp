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
#include <sstream>
#include <assert.h>

#if __linux
#  include <linux/serial.h>
#endif
 
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

    namespace {

        // Converts an integer baud-rate into a c-style '#define'd baudrate
        int cBaudrate( int baudRate )
        {
            switch(baudRate)
            {
            case 0:
                return B0;
            case 50:
                return B50;
            case 75:
                return B75;
            case 110:
                return B110;
            case 134:
                return B134;
            case 150:
                return B150;
            case 200:
                return B200;
            case 300:
                return B300;
            case 600:
                return B600;
            case 1200:
                return B1200;
            case 1800:
                return B1800;
            case 2400:
                return B2400;
            case 4800:
                return B4800;
            case 9600:
                return B9600;
            case 19200:
                return B19200;
            case 38400:
                return B38400;
            case 57600:
                return B57600;
            case 115200:
                return B115200;
#ifdef __linux            
            case 230400:
                return B230400;
            case 460800:
                return B460800;
            case 500000:
                return B500000;
            case 576000:
                return B576000;
            case 921600:
                return B921600;
            case 1000000:
                return B1000000;
            case 1152000:
                return B1152000;
            case 1500000:
                return B1500000;
            case 2000000:
                return B2000000;
            case 2500000:
                return B2500000;
            case 3000000:
                return B3000000;
            case 3500000:
                return B3500000;
            case 4000000:
                return B4000000;
#endif
            default:
                stringstream ss;
                ss << "Serial::baud() Invalid baud rate: " << baudRate;
                throw SerialException( ss.str() );
            }            
        }
    }
    
Serial::Serial( const std::string &dev,
                int baudRate,
                bool blockingMode )
    : dev_(dev),
      portFd_(-1),
      timeoutSec_(0),
      timeoutUSec_(0),
      blockingMode_(blockingMode),
      debugLevel_(0)
{
    open();
    setBaudRate( baudRate );
}
    
Serial::~Serial()
{
    close();
}

void
Serial::close()
{
    if ( debugLevel_ > 0 )
        cout<<"TRACE(serial.cpp): close()" << endl;

    assert( portFd_ != -1 );

    if(tcdrain(portFd_))
    {
        perror("Serial::close():tcdrain()");
    }
    if (::close(portFd_))
    {
        perror("Serial::close():close()");
    }
}

#ifdef __linux

void 
Serial::setBaudRate(int baud)
{
    if ( debugLevel_ > 0 )
        cout<<"TRACE(serial.cpp): setBaudRate()" << endl;

    if(portFd_==-1)
    {
        throw SerialException( "Serial:baud() no valid device open" );
    }
    if(tcgetattr(portFd_, &serialOptions_) == -1)
    {
        stringstream ss;
        ss << "Serial::baud():tcgetattr() Error reading attr: " << strerror(errno);
        throw SerialException( ss.str() );
    }

    cfsetispeed(&serialOptions_, cBaudrate(baud));
    cfsetospeed(&serialOptions_, cBaudrate(baud));

    //
    // AlexB: This code doesn't seem to work for USB devices...
    //        See: http://ozlabs.org/pipermail/linuxppc-embedded/2005-February/016848.html
    //
    if ( strstr( dev_.c_str(), "USB" ) == 0 )
    {
        //
        // AlexB: For reasons I don't fully understand, this chunk is required to make the
        //        laser work at standard baud rates.
        //
        struct serial_struct  serinfo;
        serinfo.reserved_char[0] = 0;
        if (ioctl(portFd_, TIOCGSERIAL, &serinfo) < 0) 
        {
            stringstream ss;
            ss << "Serial::setBaudRate("<<baud<<"): error calling 'ioctl(portFd_, TIOCGSERIAL, &serinfo)': "<<strerror(errno);
            throw SerialException( ss.str() );
        }
    
        serinfo.flags &= ~ASYNC_SPD_CUST;
        serinfo.custom_divisor = 0;
    
        if (ioctl(portFd_, TIOCSSERIAL, &serinfo) < 0) 
        {
            stringstream ss;
            ss << "Serial::setBaudRate("<<baud<<"): error calling 'ioctl(portFd_, TIOCSSERIAL, &serinfo)': "<<strerror(errno);
            throw SerialException( ss.str() );
        }
    }

    if ( tcsetattr(portFd_, TCSAFLUSH, &serialOptions_) == -1 )
    {
        stringstream ss;
        ss << "Serial::baud():tcsetattr() Error setting attr: " << strerror(errno);
        throw SerialException( ss.str() );
    }
}

void 
Serial::open(int flags)
{
    if ( !blockingMode_ )
        flags |= O_NONBLOCK;

    portFd_ = ::open(dev_.c_str(), flags|O_RDWR|O_NOCTTY);
    if ( portFd_ == -1 )
    {
        stringstream ss;
        ss << "Serial::open(): failed to open '"<<dev_<<"': "<<strerror(errno);
        throw SerialException( ss.str() );
    }

    if(tcgetattr(portFd_, &serialOptions_) == -1)
    {
        close();
        throw SerialException( std::string("Serial::open(): tcgetattr(): ")+strerror(errno) );
    }

    // enable receiver & ignore control lines
    serialOptions_.c_cflag |=  (CLOCAL | CREAD) ;

    // set 8 data bits
    serialOptions_.c_cflag &= ~CSIZE;
    serialOptions_.c_cflag |= CS8;

    // set parity to none with no stop bit
    serialOptions_.c_cflag &= ~PARENB;
    serialOptions_.c_cflag &= ~CSTOPB;

    // disable hardware flow control
    serialOptions_.c_cflag &= ~CRTSCTS;

#if defined(__sun)
    // http://www.sunmanagers.org/pipermail/summaries/2005-October/006871.html
    serialOptions_.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    serialOptions_.c_oflag &= ~OPOST;
    serialOptions_.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    serialOptions_.c_cflag &= ~(CSIZE|PARENB);
    serialOptions_.c_cflag |= CS8;
#else
    cfmakeraw(&serialOptions_);
#endif

    if(tcsetattr(portFd_, TCSAFLUSH, &serialOptions_) == -1)
    {
        close();
        throw SerialException( std::string("Serial::open(): tcsetattr(): ")+strerror(errno) );
    }
}

int 
Serial::read(void *buf, size_t count)
{
    if ( debugLevel_ > 0 )
        cout<<"TRACE(serial.cpp): read()" << endl;

    int got;
    got = ::read(portFd_, buf, count);
    if ( got < 0 )
    {
        throw SerialException( string("Serial::read(): ") + strerror(errno) );
    }
    return got;
}

int
Serial::readFullBlocking(void *buf, size_t count)
{
    if ( debugLevel_ > 0 )
        cout<<"TRACE(serial.cpp): readFullBlocking(): count=" << count << endl;

    int got=0;
    while( got < (int)count )
    {
        char *offset=(char*)buf+got;
        int ret = read( offset, count-got );
        got += ret;
    }
    return got;
}

int
Serial::readFullNonblocking(void *buf, size_t count)
{
    if ( debugLevel_ > 0 )
        cout<<"TRACE(serial.cpp): readFullNonblocking(): count=" << count << endl;

    int got=0;
    while ( got < (int) count ) 
    {
        char *offset=(char*)buf+got;
        int ret = ::read(portFd_, offset, count-got);
        if ( ret >= 0 )
        {
            got += ret;
        }
        else if ( ret == -1 && errno == EAGAIN )
        {
            // No data available yet -- if timeout is set we will block
            fd_set rfds;
            struct timeval tv;
            FD_ZERO(&rfds);
            FD_SET(portFd_, &rfds);
            tv.tv_sec = timeoutSec_;
            tv.tv_usec = timeoutUSec_;
            int selval = select(portFd_+1, &rfds, NULL, NULL, &tv);
            if ( selval == 0 )
            {
                // select timed out: no data
                return -1;
            }
            else if ( selval < 0 )
            {
                throw SerialException( std::string("Serial::readFullNonblocking: ")+strerror(errno) );
            }
        }
        else
        {
            throw SerialException( std::string("Serial::readFullNonblocking: read(): ")+strerror(errno) );
        }
    }
    return got;
}

int 
Serial::readFull(void *buf, size_t count)
{
    if ( blockingMode_ )
        return readFullBlocking( buf, count );
    else
        return readFullNonblocking( buf, count );
}

int 
Serial::readLineBlocking(void *buf, size_t count, char termchar)
{
    if ( debugLevel_ > 0 )
        cout<<"TRACE(serial.cpp): readLineBlocking()" << endl;

    int got = 0;
    char lastchar=0;
    do{
        //not enough room in buffer
        if(got==(int)count-1)
        {
            throw SerialException( "Serial::readLineBlocking(): Not enough room in buffer" );
        }
        char *offset=(char*)buf+got;
        int ret = ::read( portFd_, offset, 1 );
        if ( ret < 0 )
        {
            throw SerialException( std::string( "Serial::readLineBlocking(): ")+strerror(errno) );
        }
        got += ret;
        lastchar=((char*)buf)[got-1];

    } while(lastchar!=termchar);

    return got;
}

int 
Serial::readLineNonblocking(void *buf, size_t count, char termchar)
{
    if ( debugLevel_ > 0 )
        cout<<"TRACE(serial.cpp): readLineNonblocking()" << endl;

   int got = 0;
    char lastchar=0;
    do 
    {
        if ( got == (int) count-1 )
        {
            throw SerialException( "Serial::readLineBlocking(): Not enough room in buffer" );
        }
        char *offset=(char*)buf+got;
        int ret = ::read(portFd_, offset, 1);
        if(ret>0)
        {
            got += ret;
            lastchar=((char*)buf)[got-1];
        }
        else if ( ret == -1 && errno == EAGAIN )
        {  
            // No data available yet -- if timeout is set we will block
            fd_set rfds;
            struct timeval tv;
            FD_ZERO(&rfds);
            FD_SET(portFd_, &rfds);
            tv.tv_sec = timeoutSec_;
            tv.tv_usec = timeoutUSec_;
            int selval = select(portFd_+1, &rfds, NULL, NULL, &tv);
            if(selval==0)
            {
                // select timed out: no data
                return -1;
            }
            if(selval<0)
            {
                throw SerialException( std::string("Serial::readFullNonblocking: select(): ")+strerror(errno) );
            }
        }
        else if ( ret < 0 )
        {
            throw SerialException( std::string("Serial::readLineNonblocking: read(): ")+strerror(errno) );            
        }
    } while(lastchar!=termchar);

    return got;
}

int 
Serial::readLine(void *buf, size_t count, char termchar)
{
    if ( blockingMode_ )
        return readLineBlocking(buf,count,termchar);
    else
        return readLineNonblocking(buf,count,termchar);
}

int 
Serial::bytesAvailable()
{
    int ret,n_read;
    ret=ioctl(portFd_,FIONREAD,&n_read);

    if(ret==-1)
    {
        throw SerialException( std::string("Serial::bytesAvailable(): ")+strerror(errno) );
    }
    return n_read;
}

int 
Serial::bytesAvailableWait()
{
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(portFd_, &rfds);
    tv.tv_sec = timeoutSec_;
    tv.tv_usec = timeoutUSec_;
    int selval = select(portFd_+1, &rfds, NULL, NULL, &tv);
    if(selval==0)
    {
        //printf("select timed out: no data\n");
        return -1;
    }
    if( selval < 0 )
    {
        throw SerialException( std::string("Serial::bytesAvailableWait(): ")+strerror(errno) );
    }
    return bytesAvailable();
}

int 
Serial::write(const void *buf, size_t count)
{
    if ( debugLevel_ > 0 )
        cout<<"TRACE(serial.cpp): write()" << endl;

    if ( count == 0 )
        throw SerialException( "Serial::write() was called with zero bytes" );

    int put;
    put = ::write(portFd_, buf, count);
    if ( put < 0 )
    {
        throw SerialException( string("Serial::write(): ")+strerror(errno) );
    }
    else if ( put == 0 )
    {
        throw SerialException( "Serial::write(): ::write() returned 0" );
    }
    if ( debugLevel_ > 1 )
    {
        cout<<"TRACE(serial.cpp): wrote " << put << " bytes" << endl;
    }

    return put;
}

int 
Serial::writeString(const char *str, size_t maxlen)
{
    if ( debugLevel_ > 0 )
        cout<<"TRACE(serial.cpp): writeString()" << endl;

    if ( maxlen == 0 )
        throw SerialException( "Serial::writeString() was called with zero bytes" );

    int toput=strnlen(str, maxlen);
    int put;
    put = ::write(portFd_, str, toput);
    if ( put < 0 )
    {
        throw SerialException( string("Serial::write(): ")+strerror(errno) );
    }
    else if ( put == 0 )
    {
        throw SerialException( "Serial::writeString(): ::write() returned 0" );
    }
    if ( debugLevel_ > 1 )
    {
        cout<<"TRACE(serial.cpp): wrote " << put << " bytes" << endl;
    }

    return put;
}

void 
Serial::flush()
{
    int ret = tcflush(portFd_,TCIOFLUSH);
    if ( ret < 0 )
    {
        throw SerialException( std::string("Serial::flush(): ")+strerror(errno) );
    }
}

void 
Serial::drain()
{
    // wait till all output sent
    if(tcdrain(portFd_))
    {
        throw SerialException( std::string("Serial::drain(): tcdrain: ")+strerror(errno) );
    }
}

#endif

#ifdef __QNX__

void
Serial::setBaudRate(int baud)
{
    int ret;
	int c;

	ret = cfsetispeed( &serialOptions_, (speed_t) baud );
	if ( ret<0 )
	{
        throw SerialException( std::string("Serial::setBaudRate: ")+strerror(errno) );
	}
	
	ret = cfsetospeed( &serialOptions_, (speed_t) baud );
	if ( ret<0 )
	{	
        throw SerialException( std::string("Serial::setBaudRate: ")+strerror(errno) );
	}
	
    ret = tcsetattr( portFd_, TCSANOW, &serialOptions_ ) ; 
	if ( ret<0 )
	{	
        throw SerialException( std::string("Serial::setBaudRate: ")+strerror(errno) );
	}

    ret = tcgetattr( portFd_, &serialOptions_ ) ;
	if ( ret<0 )
	{	
        throw SerialException( std::string("Serial::setBaudRate: ")+strerror(errno) );
	}
 
 	// c = (int)cfgetospeed( &serialOptions_ ) ;
    cfgetospeed( &serialOptions_ ) ;
}

// flags are not used here
void 
Serial::open(const int flags)
{
	int ret;
	int baud = 9600;
 
    // argument to modem_open requires a non_const pointer
	portFd_ = modem_open( (const char*)dev_.c_str(), baud ) ;
	if( portFd_ < 0)
	{  
        printf("ERROR(serial.c): Could not open serial device\n");
        throw SerialException( "ERROR(serial.c): Could not open serial device." );
	} 

	ret = tcgetattr( portFd_, &serialOptions_ ) ;
	
	serialOptions_.c_cflag = CS8|CREAD|HUPCL|CLOCAL ;  //|IHFLOW 
	serialOptions_.c_lflag =IEXTEN ;
	serialOptions_.c_oflag =0 ;
	serialOptions_.c_iflag = IGNBRK |  IGNPAR ;  //IGNCR |

	ret = tcsetattr( portFd_, TCSANOW, &serialOptions_ ) ;
}


int 
Serial::read(void *buf, size_t count)
{
	throw SerialException("WARNING(serial.cpp::read()): NOT IMPLEMENTED YET FOR QNX");
}

int 
Serial::readLine(void *buf, size_t count, char termchar)
{
    throw SerialException("WARNING(serial.cpp::read_line()): NOT IMPLEMENTED YET FOR QNX");
}

int 
Serial::readFull(void *buf, size_t count)
{
    // these timeouts and elapsedTime do not have to be accurate so can just
	// use ints
	int timeOut = timeoutSec_ + timeoutUSec_/1000000;
	// cout << "timeout: " << timeOut << endl;

	// total time elapsed while trying to read from serial device
	int elapsedTime;

	// total number of bytes read 
	int got = 0;
    
	// number of bytes read in one readcond() function call 
	int ret = 0;
	
    // maximum number of bytes to read in one readcond() function call
	int max = 1000;

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
		
		ret = ::readcond(portFd_, (void*)offset, countChunk, countChunk, timeOut, timeOut );
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
	
	//int n = readcond(portFd_, buf, count, count, timeOut, timeOut);
	// cout << "got: " << got << endl;
	return(got) ;
}

int 
Serial::bytesAvailable()
{	
    int ret,n_read;
    ret = ioctl(portFd_,FIONREAD,&n_read);

    if( ret==-1 )
	{
        throw SerialException( std::string("Serial::bytesAvailable: ")+strerror(errno) );
    }
    return n_read;

// return( tcischars(portFd_) );	 
}

int 
Serial::bytesAvailableWait()
{
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(portFd_, &rfds);
    tv.tv_sec = timeoutSec_;
    tv.tv_usec = timeoutUSec_;
    int selval = select(portFd_+1, &rfds, NULL, NULL, &tv);
    if(selval==0)
    {
        //printf("select timed out no data\n");
        return -1;
    }
    if(selval<0){
        //perror("select()");
        throw SerialException( string("Serial::bytesAvailableWait(): select(): ")+strerror(errno) );
    }
    return bytesAvailable();
}

int 
Serial::write(const void *buf, size_t count)
{
    ssize_t put;
	
	put = ::write( portFd_, buf, count );	
	// *nw = n ;
    if ( put < 0 )
    {
        throw SerialException( string("Serial::write(): ")+strerror(errno) );
    }
    return put ;
}

int 
Serial::writeString(const char *str, size_t maxlen)
{
    int toput=strnlen(str, maxlen);
    int put;
    put = ::write(portFd_, str, toput);
    if ( put < 0 )
    {
        throw SerialException( string("Serial::writeString(): ")+strerror(errno) );
    }
    return put;
}

void 
Serial::flush()
{
	tcflush(portFd_,TCIOFLUSH);
}

void
Serial::drain()
{
    // wait till all output sent
    if(tcdrain(portFd_))
    {
        throw SerialException( string("Serial::drain():tcdrain(): ")+strerror(errno) );
    }
}


#endif


} // namespace
