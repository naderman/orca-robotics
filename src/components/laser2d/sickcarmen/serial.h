/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
/* AlexB: This code was copied directly from CARMEN, but may have diverged over time. */

#ifndef SERIAL_H
#define SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

int carmen_serial_connect(int *dev_fd, char *dev_name);

void carmen_serial_configure(int dev_fd, int baudrate, char *parity);

// Returns the number of characters available for reading from this fd.
long carmen_serial_numChars(int dev_fd);

int carmen_serial_ClearInputBuffer(int dev_fd);

int carmen_serial_writen(int dev_fd, char *buf, int nChars);

int carmen_serial_readn(int dev_fd, char *buf, int nChars);

#ifdef __cplusplus
}
#endif

#endif
