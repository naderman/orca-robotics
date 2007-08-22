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

class LockFile;

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

    //! Opens a device @ref dev.
    //! Throws exceptions on error.
    //! If useLockFile is set to true, Serial will use the file-system to 
    //! prevent concurrent access to a serial device by multiple instances of Serial.
    Serial( const std::string &dev,
            int  baudRate,
            bool enableTimeouts,
            int  debuglevel = 0,
            bool useLockFile = true );

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
    ssize_t read(void *buf, int count);

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
    ssize_t readFull(void *buf, int count);

    //! Reads a line of data up to @ref count bytes-1 (including @ref termchar), terminated by @ref termchar.
    //! Returns the number of bytes read.
    //! After reading the line then the string will be NULL terminated.
    //!
    //! Example: if you expect to read the string "1234\n", you need something like:
    //!                char buf[6];
    //!                serial.readLine( buf, 6 );
    //!
    //!          where the two extra characters are for the '\n' and the terminating '\0'.
    //!
    //! If timeouts are not enabled we might block forever, waiting for the number of bytes we want or an error.
    //!
    //! If timeouts are enabled we won't block more than the timeout specified.
    //! Returns -1 if it timed out.
    //! NOTE: The timeout applies for each individual read() call.  We might have to make lots of them,
    //!       so the total time for which this function blocks might be longer than the specified timeout.
    //!
    ssize_t readLine(void *buf, int count, char termchar='\n');

    //! Returns the number of bytes available for reading (non-blocking).
    int bytesAvailable();

    //! Returns the number of bytes available for reading.  Waits according to the timeout.
    //! Returns:
    //! - <0 :  timed out
    //! - >0 :  data ready
    int bytesAvailableWait();

    //! Writes some data.  Returns the number of bytes written.
    ssize_t write(const void *buf, int count);

    //! Writes a ('\0'-terminated) string. 
    //! Returns the number of bytes written.
    ssize_t writeString(const char *buf);
    inline ssize_t writeString(const std::string &s) {
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
    //  TIMED_OUT: timed out
    //  GOT_DATA : data available
    int waitForDataOrTimeout(void);

    // Opens a device @ref dev.
    void open(int flags=0);

    // Won't throw exceptions.
    void close();

    const std::string dev_;
    int portFd_;
    int timeoutSec_;
    int timeoutUSec_;
    bool timeoutsEnabled_;
    
    int debugLevel_;

    LockFile *lockFile_;
};

}

#endif
