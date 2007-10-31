/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LOGGER_AUTO_LOGGER_H
#define ORCA2_LOGGER_AUTO_LOGGER_H

namespace orcalog
{

//! An AutoLogger simply logs continuously once it's started.    
class AutoLogger {
public:
    virtual ~AutoLogger() {}
    
    virtual void initAndStartLogging()=0;
};

}

#endif
