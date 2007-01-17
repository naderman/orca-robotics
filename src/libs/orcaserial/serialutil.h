 /*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef _ORCA_SERIALUTIL_H_
#define _ORCA_SERIALUTIL_H_

#ifdef __QNX__

#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string>

namespace orcaserial {

//!
//! @brief provides some useful serial tools  
//!

int waitForByte( char byte, int timeout, int port_fd );


} // namespace

#endif

#endif
