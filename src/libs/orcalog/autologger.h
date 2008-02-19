/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_LOGGER_AUTO_LOGGER_H
#define ORCA_LOGGER_AUTO_LOGGER_H

#include <orcalog/logwriterinfo.h>
#include <orcalog/masterfilewriter.h>

namespace orcalog
{

//! An AutoLogger simply logs continuously once it's started.    
class AutoLogger {
public:
    virtual ~AutoLogger() {}
    
    virtual void init( const LogWriterInfo &logWriterInfo, MasterFileWriter &masterFileWriter )=0;
    virtual void startLogging()=0;
};

}

#endif
