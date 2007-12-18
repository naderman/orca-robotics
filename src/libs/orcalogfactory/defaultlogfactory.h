#if 0
/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_LOGGER_DEFAULT_AUTO_LOGGER_FACTORY_H
#define ORCA_LOGGER_DEFAULT_AUTO_LOGGER_FACTORY_H

#include <orcalog/autologgerfactory.h>

namespace orcalogfactory
{

class DefaultAutoLoggerFactory: public orcalog::AutoLoggerFactory

{
public:
    DefaultAutoLoggerFactory();
    
    virtual orcalog::IAutoLogger* create( const LogWriterInfo &logWriterInfo );
};

} // namespace

extern "C" {
    orcalog::AutoLoggerFactory *createAutoLoggerFactory();
}


#endif
#endif
