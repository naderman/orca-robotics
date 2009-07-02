/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_LOGGER_AUTO_LOGGER_FACTORY_H
#define ORCA_LOGGER_AUTO_LOGGER_FACTORY_H

#include <orcalog/factory.h>
#include <orcalog/autologger.h>
#include <orcalog/logwriterinfo.h>
#include <orcaice/context.h>

namespace orcalog
{

//! Factory of AutoLogger instances.
class AutoLoggerFactory : public Factory
{
public:
    virtual ~AutoLoggerFactory() {};

    //! Creates an AutoLogger with the details specified. Returns a pointer to it if
    //! successful or a NULL pointer if it fails.
    virtual AutoLogger* create( const std::string &interfaceType, const orcaice::Context& context )=0;
};

} // namespace


// Function for instantiating plug-in factories.
// A plug-in library must have a function like so:
// extern "C" {
//     orcalog::AutoLoggerFactory *createAutoLoggerFactory();
// }
typedef orcalog::AutoLoggerFactory *AutoLoggerFactoryMakerFunc();


#endif
