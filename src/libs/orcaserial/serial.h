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
//! Warning: this thing is _NOT_ thread-safe.
//!
//! @author Matthew Ridley, Alex Brooks
//!
class Serial
{
public:

    //! Constructor.  
    //! Opens a device @ref dev.
    //! Throws exceptions on error
    Serial( const std::string &dev, int baudRate, bool blockingMode );

    //! Destructor closes serial port
    ~Serial();

    //! turn on/off debug messages
    void setDebugLevel( int debugLevel ) { debugLevel_ = debugLevel; }

    //! Sets the baud rate. Flushes any data.
    void setBaudRate(int baud);

    //! Sets timeout for non-blocking operations.
    void setTimeout(int sec, int usec) { timeoutSec_=sec; timeoutUSec_=usec; };

    //! Reads up to @ref count bytes into buffer @ref buf.
    //! Returns the number of bytes read.
    //! Will never return <0 -- throws exceptions instead.
    //! In blocking mode, blocks till it gets something.
    //! In non-blocking mode, throws an exception if data isn't available.
    int read(void *buf, size_t count);

    //! Tries to read exactly @ref count bytes into @ref buf.  
    //! Returns the number of bytes read, or throws an exception.
    //!
    //! In blocking mode we might block forever, waiting for the number of bytes we want or an error.
    //!
    //! In non-blocking mode we won't block more than the timeout specified.
    //! Returns -1 if it timed out.
    //! NOTE: The timeout applies for each individual read() call.  We might have to make lots of them,
    //!       so the total time for which this function blocks might be longer than the specified timeout.
    //!
    int readFull(void *buf, size_t count);

    //! Reads a line of data up to @ref count bytes-1 (including @ref termchar), terminated by @ref termchar.
    //! Returns the number of bytes read.
    //! After reading the line then the string will be NULL terminated.
    //!
    //! In blocking mode we might block forever, waiting for the number of bytes we want or an error.
    //!
    //! In non-blocking mode we won't block more than the timeout specified.
    //! Returns -1 if it timed out.
    //! NOTE: The timeout applies for each individual read() call.  We might have to make lots of them,
    //!       so the total time for which this function blocks might be longer than the specified timeout.
    //!
    int readLine(void *buf, size_t count, char termchar='\n');

    //! Returns the number of bytes available for reading (non-blocking).
    int bytesAvailable();

    //! Returns the number of bytes available for reading.  Waits according to the timeout.
    //! Returns:
    //! - <0 :  timed out
    //! - >0 :  data ready
    int bytesAvailableWait();

    //! Writes some data.  Returns the number of bytes written.
    int write(const void *buf, size_t count);

    //! Writes a ('\0'-terminated) string (default up to 256 chars). 
    //! Returns the number of bytes written.
    int writeString(const char *buf, size_t maxlen=256);

    //! Flushs both input and output buffers.
    //! This discards all data in buffers.
    void flush();

    //! Finishes transmission from output buffers and drains input buffers.
    void drain();

    //! This gives direct access to the file descriptor: be careful with this...
    int fileDescriptor() { return portFd_; }

private:

    // Utility function to do the blocking for the file accesses
    int doBlocking(void);

    // Opens a device @ref dev.
    void open(int flags=0);

    // Won't throw exceptions.
    void close();

    int readFullBlocking(void *buf, size_t count);
    int readFullNonblocking(void *buf, size_t count);

    const std::string dev_;
    struct termios serialOptions_;
    int portFd_;
    int timeoutSec_;
    int timeoutUSec_;
    bool blockingMode_;
    
    int debugLevel_;
};

}

#endif
