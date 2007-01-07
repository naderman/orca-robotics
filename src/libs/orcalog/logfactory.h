/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LOGGER_FACTORY_H
#define ORCA2_LOGGER_FACTORY_H

#include <orcalog/factory.h>

namespace orcaice
{
    class Context;
}

namespace orcalog
{

class Logger;
class LogMaster;

//! Factory of slave loggers.
class LogFactory : public Factory
{
public:
    virtual ~LogFactory() {};

    //! Creates a slave logger of specified type. Returns a pointer to it if
    //! successful or a NULL pointer if it fails.
    virtual Logger* create( const std::string      &interfaceType, 
                           const std::string      &typeSuffix,
                           const std::string      &format,
                           LogMaster              *master,
                           const std::string      &filenamePrefix,
                           const orcaice::Context &context ) = 0;
};

} // namespace


// Function for instantiating plug-in factories.
// A plug-in library must have a function like so:
// extern "C" {
//     orcalog::LogFactory *createLogFactory();
// }
typedef orcalog::LogFactory *LogFactoryMakerFunc();


#endif
