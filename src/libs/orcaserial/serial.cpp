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
#include <iomanip>
#include <sstream>
#include <assert.h>
#include "serial.h"
#include "serialutil.h"


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

// define for a debug compile
#define DEBUG

 
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

        int iBaudrate( int baudRate )
        {
            switch(baudRate)
            {
            case B0:
                return 0;
            case B50:
                return 50;
            case B75:
                return 75;
            case B110:
                return 110;
            case B134:
                return 134;
            case B150:
                return 150;
            case B200:
                return 200;
            case B300:
                return 300;
            case B600:
                return 600;
            case B1200:
                return 1200;
            case B1800:
                return 1800;
            case B2400:
                return 2400;
            case B4800:
                return 4800;
            case B9600:
                return 9600;
            case B19200:
                return 19200;
            case B38400:
                return 38400;
            case B57600:
                return 57600;
            case B115200:
                return 115200;
            case B230400:
                return 230400;
            case B460800:
                return 460800;
            case B500000:
                return 500000;
            case B576000:
                return 576000;
            case B921600:
                return 921600;
            case B1000000:
                return 1000000;
            case B1152000:
                return 1152000;
            case B1500000:
                return 1500000;
            case B2000000:
                return 2000000;
            case B2500000:
                return 2500000;
            case B3000000:
                return 3000000;
            case B3500000:
                return 3500000;
            case B4000000:
                return 4000000;
            default:
                stringstream ss;
                ss << "Serial::baud() Invalid baud rate: " << baudRate;
                throw SerialException( ss.str() );
            }            
        }
    }





    
Serial::Serial( const std::string &dev,
                int baudRate,
                bool blockingMode, 
                int debuglevel)
    : dev_(dev),
      portFd_(-1),
      timeoutSec_(0),
      timeoutUSec_(0),
      blockingMode_(blockingMode),
      debugLevel_(debuglevel)
{
    open();
    setBaudRate( baudRate );
    
    if(debugLevel_ > 1){
        showStatus("At end of Serial::Serial:");
    }
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

    //Make sure that we force this back to an invalid state
    portFd_ = -1;
}

#ifdef __linux

void 
Serial::setBaudRate(int baud)
{
    struct termios localOptions;

    if ( debugLevel_ > 0 )
        cout<<"TRACE(serial.cpp): setBaudRate()" << endl;

    if(portFd_==-1)
    {
        throw SerialException( "Serial:baud() no valid device open" );
    }
    if(tcgetattr(portFd_, &localOptions) == -1)
    {
        stringstream ss;
        ss << "Serial::baud():tcgetattr() Error reading attr: " << strerror(errno);
        throw SerialException( ss.str() );
    }

    cfsetispeed(&localOptions, cBaudrate(baud));
    cfsetospeed(&localOptions, cBaudrate(baud));

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

    if ( tcsetattr(portFd_, TCSAFLUSH, &localOptions) == -1 )
    {
        stringstream ss;
        ss << "Serial::baud():tcsetattr() Error setting attr: " << strerror(errno);
        throw SerialException( ss.str() );
    }
}

void 
Serial::open(int flags)
{
    struct termios localOptions;
    
    if ( !blockingMode_ )
        flags |= O_NONBLOCK;

    portFd_ = ::open(dev_.c_str(), flags|O_RDWR|O_NOCTTY);
    if ( portFd_ == -1 )
    {
        stringstream ss;
        ss << "Serial::open(): failed to open '"<<dev_<<"': "<<strerror(errno);
        throw SerialException( ss.str() );
    }


    

    cout << "TODO remove: Going to tcgetattr in open() for " << dev_ <<endl;

    if(tcgetattr(portFd_, &localOptions) == -1)
    {
        close();
        throw SerialException( std::string("Serial::open(): tcgetattr(): ")+strerror(errno) );
    }

//TODO is this the best way of having this code conditional? 
    if(debugLevel_ > 1){
        cout << "Displaying status"<<endl;
        showStatus("At beginning of Open():");
    }



    // enable receiver & ignore control lines
    localOptions.c_cflag |=  (CLOCAL | CREAD) ;

    // set 8 data bits
    localOptions.c_cflag &= ~CSIZE;
    localOptions.c_cflag |= CS8;

    // set parity to none with no stop bit
    localOptions.c_cflag &= ~PARENB;
    localOptions.c_cflag &= ~CSTOPB;

    // disable hardware flow control
    localOptions.c_cflag &= ~CRTSCTS;

#if defined(__sun)
    // http://www.sunmanagers.org/pipermail/summaries/2005-October/006871.html
    localOptions.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    localOptions.c_oflag &= ~OPOST;
    localOptions.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    localOptions.c_cflag &= ~(CSIZE|PARENB);
    localOptions.c_cflag |= CS8;
#else
    cfmakeraw(&localOptions);
#endif

    if(tcsetattr(portFd_, TCSAFLUSH, &localOptions) == -1)
    {
        close();
        throw SerialException( std::string("Serial::open(): tcsetattr(): ")+strerror(errno) );
    }

    if ( debugLevel_ > 1 ){
        showStatus("At end of Open():");
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
                // select timed out: no data, or an error occured
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






//////////////////////////////////////
//Start Duncan rewrite...
//DUNCAN; Combined blocking and non-blocking...



int 
Serial::readLine(void *buf, size_t count, char termchar)
{
    if ( debugLevel_ > 0 ){
        cout<<"TRACE(serial.cpp): readLine ";
        if(blockingMode_){ 
            cout << "In blocking mode"<<endl; 
        }else{
            cout << "Non blocking mode"<<endl;
        }
    }
    //There must be at least room for a terminating char and NULL terminator!
    assert (count >= 2);

    char* dataPtr = static_cast<char*>(buf);
    char nextChar = 0;

    do{        
        //Check for buf overrun Must leave room for NULL terminator
        if(dataPtr >= (static_cast<char*>(buf) + (count - 1)))
        {
            throw SerialException( "Serial::readLine: Not enough room in buffer" );
        }

        int ret = ::read( portFd_, &nextChar, 1 );
        if (ret == 1)
        {
            *(dataPtr++) = nextChar; //got data let's store it...
        }
        else if ( ret < 0 )
        {

            //If blocking and no data, wait and then go again
            if(blockingMode_ &&  (ret == -1) && (errno == EAGAIN) ){  
                if(doBlocking() != -1)
                {
                    continue;
                }else{
                    *dataPtr = 0x00; //terminate string just incase it's used anyway
                    return -1;
                }
            }

            //If we get here then it was a more serious error
            throw SerialException( std::string( "Serial::readLine(): ")+strerror(errno) );
        }

    }while (nextChar != termchar);

//It's a string. It must be NULL terminated...
    *dataPtr = 0x00;

//Return the number of chars not including the NULL
    return ( static_cast<int> (dataPtr - static_cast<char*>(buf)) );

 
//TODO: Duncan! I think that this should cope with any <CR><LF> pair gracefully!

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
    if ( doBlocking() == -1){
        return -1;
    }

    return bytesAvailable();
}


int 
Serial::doBlocking()
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
        throw SerialException( std::string("Serial::doBlocking: select(): ")+strerror(errno) );
    }
    
    return 0;
}


//TODO: Duncan can we ditch the un-needed length field (ignore_this_field)?
int 
Serial::writeString(const char *str, int ignore_this_field)
{
    (void) ignore_this_field;

    if ( debugLevel_ > 0 )
        cout<<"TRACE(serial.cpp): writeString()" << endl;

    int put;
    put = ::write(portFd_, str, strlen(str) );
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
Serial::showStatus(std::string identify){
    struct termios status;
    if(tcgetattr(portFd_, &status) == -1)
    {
        close();
        throw SerialException( std::string(identify + "tcgetattr():")+strerror(errno) );
    }

    cout << endl << identify << " Device " << dev_ << " Status:-" << endl;
    cout << "In baud_rate: " << iBaudrate(cfgetispeed(&status)) 
         << " Out baud_rate: " << iBaudrate(cfgetospeed(&status)) << endl;
    cout << status;
    
    showFdState();
}


//TODO This was supposed to be in serialutil but I couldn't get it to
//compile (Duncan)
//Allow streaming of the term status structure type
std::ostream &operator<<( std::ostream &s, struct termios &stat ){

    s.setf(ios::hex,ios::basefield);
    s.fill('0');

    s << "c_iflag -> 0x"  << setw(4) <<  stat.c_iflag << endl;
    s << "c_oflag -> 0x"  << setw(4) <<  stat.c_oflag << endl;
    s << "c_cflag -> 0x"  << setw(4) <<  stat.c_cflag << endl;
    s << "c_lflag -> 0x"  << setw(4) <<  stat.c_lflag << endl;

    s << "c_cc_array ->" << endl;
    s << "VINTR 0x"   << setw(2) <<  static_cast<int>(stat.c_cc[VINTR])  
      << ", VQUIT 0x" << setw(2) <<  static_cast<int>(stat.c_cc[VQUIT]) 
      << ", VERASE 0x" << setw(2) <<  static_cast<int>(stat.c_cc[VERASE]) 
      << ", VKILL 0x"  << setw(2) <<  static_cast<int>(stat.c_cc[VKILL]) << endl ;
    s << "VEOF 0x"   << setw(2) <<  static_cast<int>(stat.c_cc[VEOF])
      << ", VEOL 0x" << setw(2) <<  static_cast<int>(stat.c_cc[VEOL])
      << ", VEOL2 0x" << setw(2) <<  static_cast<int>(stat.c_cc[VEOL2]) 
      << ", VSTART 0x" << setw(2) <<  static_cast<int>(stat.c_cc[VSTART]) << endl ;
    s << "VSTOP 0x"   << setw(2) <<  static_cast<int>(stat.c_cc[VSTOP])
      << ", VSUSP 0x" << setw(2) <<  static_cast<int>(stat.c_cc[VSUSP])
      << ", VREPRINT 0x" << setw(2) <<  static_cast<int>(stat.c_cc[VREPRINT])
      << ", VLNEXT 0x" << setw(2) <<  static_cast<int>(stat.c_cc[VLNEXT]) << endl;
    s << "VMIN 0x" << setw(2) <<  static_cast<int>(stat.c_cc[VMIN])
      << ", VTIME 0x"   << setw(2) <<  static_cast<int>(stat.c_cc[VTIME]) << endl;
   

    s << "c_iflag, Bits set" << endl;
    if (stat.c_iflag & IGNBRK){ s << "IGNBRK,";}
    if (stat.c_iflag & BRKINT){ s << "BRKINT,";}
    if (stat.c_iflag & IGNPAR){ s << "IGNPAR,";}
    if (stat.c_iflag & PARMRK){ s << "PARMRK,";}
    if (stat.c_iflag & INPCK){ s << "INPCK,";}
    if (stat.c_iflag & ISTRIP){ s << "ISTRIP,";}
    if (stat.c_iflag & INLCR){ s << "INLCR,";}
    if (stat.c_iflag & IGNCR){ s << "IGNCR,";}
    if (stat.c_iflag & ICRNL){ s << "ICRNL,";}
    if (stat.c_iflag & IUCLC){ s << "IUCLC,";}
    if (stat.c_iflag & IXON){ s << "IXON,";}
    if (stat.c_iflag & IXANY){ s << "IXANY,";}
    if (stat.c_iflag & IXOFF){ s << "IXOFF,";}
    if (stat.c_iflag & IMAXBEL){ s << "IMAXBEL,";}
    s << endl;

    s << "c_oflag, Bits set" << endl;
    if (stat.c_oflag & OPOST){ s << "OPOST,";}
    if (stat.c_oflag & OLCUC){ s << "OLCUC,";}
    if (stat.c_oflag & ONLCR){ s << "ONLCR,";}
    if (stat.c_oflag & OCRNL){ s << "OCRNL,";}
    if (stat.c_oflag & ONOCR){ s << "ONOCR,";}
    if (stat.c_oflag & ONLRET){ s << "ONLRET,";}
    if (stat.c_oflag & OFILL){ s << "OFILL,";}
    if (stat.c_oflag & OFDEL){ s << "OFDEL,";}
    if (stat.c_oflag & NLDLY){ s << "NLDLY,";}
    if (stat.c_oflag & CRDLY){ s << "CRDLY,";}
    if (stat.c_oflag & TABDLY){ s << "TABDLY,";}
    if (stat.c_oflag & BSDLY){ s << "BSDLY,";}
    if (stat.c_oflag & VTDLY){ s << "VTDLY,";}
    if (stat.c_oflag & FFDLY){ s << "FFDLY,";}
    s << endl;

    s << "c_cflag, Bits set" << endl;
    if (stat.c_cflag & CSTOPB){ s << "CSTOPB,";}
    if (stat.c_cflag & CREAD){ s << "CREAD,";}
    if (stat.c_cflag & PARENB){ s << "PARENB,";}
    if (stat.c_cflag & PARODD){ s << "PARODD,";}
    if (stat.c_cflag & HUPCL){ s << "HUPCL,";}
    if (stat.c_cflag & CLOCAL){ s << "CLOCAL,";}
    if (stat.c_cflag & CIBAUD){ s << "CIBAUD,";}
    s << endl;


    s << "c_lflag, Bits set" << endl;
    if (stat.c_lflag & ISIG){ s << "ISIG,";}
    if (stat.c_lflag & ICANON){ s << "ICANON,";}
    if (stat.c_lflag & PARENB){ s << "PARENB,";}
    if (stat.c_lflag & XCASE){ s << "XCASE,";}
    if (stat.c_lflag & NOFLSH){ s << "NOFLSH,";}
    if (stat.c_lflag & FLUSHO){ s << "FLUSHO,";}
    if (stat.c_lflag & PENDIN){ s << "PENDIN,";}
    if (stat.c_lflag & IEXTEN){ s << "IEXTEN,";}
    if (stat.c_lflag & TOSTOP){ s << "TOSTOP,";}
    if (stat.c_lflag & ECHO){ s << "ECHO,";}
    if (stat.c_lflag & ECHOE){ s << "ECHOE,";}
    if (stat.c_lflag & ECHOPRT){ s << "ECHOPRT,";}
    if (stat.c_lflag & ECHOKE){ s << "ECHOKE,";}
    if (stat.c_lflag & NOFLSH){ s << "ECHONL,";}
    if (stat.c_lflag & FLUSHO){ s << "ECHOCTL,";}
    s << endl;

    s << endl;
    s.setf(ios::dec,ios::basefield);


}


void 
Serial::showFdState(){

    struct serial_struct  serinfo;
    if (ioctl(portFd_, TIOCGSERIAL, &serinfo) < 0) 
    {
        stringstream ss;
        ss << "error calling 'ioctl(portFd_, TIOCGSERIAL, &serinfo)': "<<strerror(errno);
        throw SerialException( ss.str() );
    }
    
    cout.setf(ios::hex,ios::basefield);

    cout << "TIOCGSERIAL:-> " << endl;
    cout << "Flags: 0x"   << setw(2) << serinfo.flags
         << ", Type: 0x" << setw(2) << serinfo.type
         << ", Line: 0x" << setw(2) << serinfo.line << endl;
    cout << "Port: 0x"   << setw(2) << serinfo.port
         << ", IRQ: 0x" << setw(2) << serinfo.irq
         << ", xmit_fifo_size: 0x" << setw(2) << serinfo.xmit_fifo_size << endl;
    cout << "Baud_base: 0x"   << setw(2) << serinfo.baud_base
         << ", Custom_divisor: 0x" << setw(2) << serinfo.custom_divisor
         << ", Io_type: 0x" << setw(2) << serinfo.io_type << endl;

    cout.setf(ios::dec,ios::basefield);

}

//// END DUNCAN REWRITE ////



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
	portFd_ = modem_open( const_cast<char*>(dev_.c_str()), baud ) ;
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
