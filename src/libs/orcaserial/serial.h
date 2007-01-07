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
//! @brief Encapsulates a serial port.  
//!
class Serial
{
public:
    //! Constructor.
    Serial();
    ~Serial();

    //! Opens a device @ref dev.
    //! O_NONBLOCK might be handy in flags.
    int open(const char *dev, const int flags);

    //! Reads some data into buffer @ref buf.
    int read(void *buf, size_t count);

    //! Reads some data (exactly @ref count bytes or error).
    //! Returns the number of bytes read, or -1 on error.
    int read_full(void *buf, size_t count);

    //! Reads a line of data up to @ref count bytes (including \0) terminated by termchar.
    //! Returns the number of bytes read, or -1 on error.
    int read_line(void *buf, size_t count, char termchar);

    //! Returns the number of bytes available for reading (non-blocking).
    int data_avail();

    //! Returns the number of bytes available for reading.  Waits according to the timeout.
    //! Returns:
    //! - 0  :  timed out
    //! - >0 :  data ready
    //! - <0 :  error
    int data_avail_wait();

    //! Writes some data.
    int write(const void *buf, size_t count);

    //! Writes a string (default up to 256 chars)
    int write(const char *buf, size_t maxlen=256);

    //! Sets the baud rate of an open device. Flushes any data.
    int baud(int baud);

    //! Sets timeout for blocking operations.
    void timeout(int sec, int usec) { to_sec=sec; to_usec=usec; };

    //! Flushs both input and output buffers.
    //! This discards all data in buffers.
    int flush();

    //! Finishes transmission from output buffers and drains input buffers.
    int drain();

    //! If any of the functions above produced an error, you can see it with this call.
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
