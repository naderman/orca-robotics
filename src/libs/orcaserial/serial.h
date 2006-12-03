/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Mathew Ridley, Alex Brooks
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
//! Encapsulates a serial port.  
//!
class Serial
{
public:
    Serial();
    ~Serial();
    //! open a device.  O_NONBLOCK might be handy in flags.
    int open(const char *dev, const int flags);
    //! read some data
    int read(void *buf, size_t count);
    //! read some data (exactly count bytes or error)
    //! returns the number of bytes read, or -1 on error
    int read_full(void *buf, size_t count);
    //! read a line of data up to count bytes (including \0) terminated by termchar
    //! returns the number of bytes read, or -1 on error
    int read_line(void *buf, size_t count, char termchar);
    //! Returns the number of bytes available for reading (non-blocking).
    int data_avail();
    //! Returns the number of bytes available for reading.  Waits according to the timeout.
    //! Returns:
    //! 0:  timed out
    //! >0: data ready
    //! <0: error
    int data_avail_wait();
    //! write some data
    int write(const void *buf, size_t count);
    //! write a string
    int write(const char *buf, size_t maxlen=256);
    //! set baud rate of an open device
    int baud(int baud);
    //! set timeout for blocking operations
    void timeout(int sec, int usec){to_sec=sec;to_usec=usec;};
    //! flush both input and output buffers
    int flush();
    //! finish transmission and drain input buffers
    int drain();

    //! If one of the functions above produce an error, you can see it with this call.
    std::string last_error() const { return lastError_; }

private:
    struct termios ser_opts;
    int port_fd;
    int to_sec;
    int to_usec;

    std::string lastError_;
};

}

#endif
