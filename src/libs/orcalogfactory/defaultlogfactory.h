/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LOGGER_DEFAULT_LOG_FACTORY_H
#define ORCA2_LOGGER_DEFAULT_LOG_FACTORY_H

#include <orcalog/logfactory.h>

namespace orcalog
{
    class LogMaster;
    class Logger;
}

namespace orcalogfactory
{

class DefaultLogFactory: public orcalog::LogFactory

{
public:
    DefaultLogFactory();
    
    virtual orcalog::Logger* create( const std::string      &interfaceType, 
                                     const std::string      &typeSuffix,
                                     const std::string      &format,
                                     orcalog::LogMaster     *master,
                                     const std::string      &filenamePrefix,
                                     const orcaice::Context &context );
};

} // namespace

extern "C" {
    orcalog::LogFactory *createLogFactory();
}


#endif
