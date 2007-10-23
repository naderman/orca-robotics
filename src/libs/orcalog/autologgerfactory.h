/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LOGGER_AUTO_LOGGER_FACTORY_H
#define ORCA2_LOGGER_AUTO_LOGGER_FACTORY_H

#include <orcalog/factory.h>
#include <orcalog/logwriterinfo.h>

namespace orcalog
{

//! An AutoLogger simply logs continuously once it's started.    
class AutoLogger {
public:
    virtual ~AutoLogger() {}
    
    virtual void initAndStartLogging()=0;
};

//! Factory of slave loggers.
class AutoLoggerFactory : public Factory
{
public:
    virtual ~AutoLoggerFactory() {};

    //! Creates an AutoLogger with the details specified. Returns a pointer to it if
    //! successful or a NULL pointer if it fails.
    virtual AutoLogger* create( const LogWriterInfo &logWriterInfo )=0;
};

} // namespace


// Function for instantiating plug-in factories.
// A plug-in library must have a function like so:
// extern "C" {
//     orcalog::AutoLoggerFactory *createAutoLoggerFactory();
// }
typedef orcalog::AutoLoggerFactory *AutoLoggerFactoryMakerFunc();


#endif
