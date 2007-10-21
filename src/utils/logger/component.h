/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LOGGER_COMPONENT_H
#define ORCA2_LOGGER_COMPONENT_H

#include <string>
#include <orcaice/component.h>
#include <hydrodll/dll.h>

namespace orcalog
{
    class LogMaster;
    class LogFactory;
}

namespace logger
{

class Component : public orcaice::Component
{
public:

    Component( const std::string &compName );
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:
    std::vector<orcalog::LogFactory*>                       logFactories_;
    std::vector<hydrodll::DynamicallyLoadedLibrary*> libraries_;

    orcalog::LogMaster *master_;

    // utilities
    void loadPluginLibraries( const std::string &factoryLibNames );
    void createLogger( const std::string  &interfaceType, 
                       const std::string  &interfaceTypeSuffix,
                       const std::string  &format,
                       const std::string  &filenamePrefix );
};

}

#endif
