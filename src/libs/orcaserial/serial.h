/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef _ORCA_SERIAL_H_
#define _ORCA_SERIAL_H_

#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string>

namespace orcaserial {

//!
//! @brief Exception thrown by Serial.
//!
class SerialException : public std::exception
{ 
    std::string  message_;
public:
    SerialException(const char *message)
        : message_(message) {}
    SerialException(const std::string &message)
        : message_(message) {}
    ~SerialException()throw(){}
    virtual const char* what() const throw() { return message_.c_str(); }
};

//!
//! @brief Encapsulates a serial port.
//!
//! This class hard-codes some options, such as:
//!  - 8 data bits
//!  - no handshaking
//!
//! Warning: this thing is _NOT_ thread-safe.
//!
//! @author Matthew Ridley, Alex Brooks
//!
class Serial
{
public:

    //! Constructor.  
    //! Opens a device @ref dev.
    //! Throws exceptions on error.
    //! Note that a default debugLevel other than 0 can be passed to the constructor
    Serial( const std::string &dev, int baudRate, bool enableTimeouts, int debuglevel = 0 );

    //! Destructor closes serial port
    ~Serial();

    //! turn on/off debug messages
    void setDebugLevel( int debugLevel ) { debugLevel_ = debugLevel; }

    //! Sets the baud rate. Flushes any data.
    void setBaudRate(int baud);

    //! Sets timeout (timeouts must be enabled)
    void setTimeout(int sec, int usec);

    //! Reads up to @ref count bytes into buffer @ref buf.
    //! Returns the number of bytes read.
    //! Will never return <0 -- throws exceptions instead.
    //! If timeouts are not enabled, blocks till it gets something.
    //! If timeouts are enabled, throws an exception if data isn't available.
    int read(void *buf, int count);

    //! Tries to read exactly @ref count bytes into @ref buf.  
    //! Returns the number of bytes read, or throws an exception.
    //!
    //! If timeouts are not enabled we might block forever, waiting for the number of bytes we want or an error.
    //!
    //! If timeouts are enabled we won't block more than the timeout specified.
    //! Returns -1 if it timed out.
    //! NOTE: The timeout applies for each individual read() call.  We might have to make lots of them,
    //!       so the total time for which this function blocks might be longer than the specified timeout.
    //!
    int readFull(void *buf, int count);

    //! Reads a line of data up to @ref count bytes-1 (including @ref termchar), terminated by @ref termchar.
    //! Returns the number of bytes read.
    //! After reading the line then the string will be NULL terminated.
    //!
    //! If timeouts are not enabled we might block forever, waiting for the number of bytes we want or an error.
    //!
    //! If timeouts are enabled we won't block more than the timeout specified.
    //! Returns -1 if it timed out.
    //! NOTE: The timeout applies for each individual read() call.  We might have to make lots of them,
    //!       so the total time for which this function blocks might be longer than the specified timeout.
    //!
    int readLine(void *buf, int count, char termchar='\n');

    //! Returns the number of bytes available for reading (non-blocking).
    int bytesAvailable();

    //! Returns the number of bytes available for reading.  Waits according to the timeout.
    //! Returns:
    //! - <0 :  timed out
    //! - >0 :  data ready
    int bytesAvailableWait();

    //! Writes some data.  Returns the number of bytes written.
    int write(const void *buf, int count);

    //! Writes a ('\0'-terminated) string. 
    //! Returns the number of bytes written.
    int writeString(const char *buf);
    inline int writeString(std::string &s) {
        return writeString(s.c_str());
    }

    //! Flushs both input and output buffers.
    //! This discards all data in buffers.
    void flush();

    //! Finishes transmission from output buffers and drains input buffers.
    void drain();

    //! This gives direct access to the file descriptor: be careful with this...
    int fileDescriptor() { return portFd_; }

    //! Print some diagnostic information about the current status of the port to cout.
    std::string getStatusString();


private:

    // Utility function to wait up to the timeout for data to appear.
    // Returns:
    //  -1: timed out
    //   0: data available
    int waitForTimeout(void);

    // Opens a device @ref dev.
    void open(int flags=0);

    // Won't throw exceptions.
    void close();

    int readFullBlocking(void *buf, int count);
    int readFullWithTimeout(void *buf, int count);

    const std::string dev_;
    int portFd_;
    int timeoutSec_;
    int timeoutUSec_;
    bool timeoutsEnabled_;
    
    int debugLevel_;
};

}

#endif
