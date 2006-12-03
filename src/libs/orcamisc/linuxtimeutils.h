/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_MISC_LINUXTIME_H
#define ORCA2_MISC_LINUXTIME_H

#include <time.h>
#include <sys/time.h>

namespace orcamisc {
  
    //! Returns the current time as Linux timeval.
	//! @note Linux only.
    timeval now();
    
    //! Returns the time difference since start time and now as Linux timeval
	//! @note Linux only.
    timeval elapsedTimeVal( timeval & startTime );
    
    //! Returns the time difference since start time in seconds
	//! @note Linux only.
    double elapsedTimeSec( timeval & startTime );

}

#endif
