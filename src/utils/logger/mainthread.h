/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <hydroiceutil/subsystemthread.h>
#include <orcaice/context.h>

#include <hydrodll/dll.h>
#include <string>
#include <memory>

namespace orcalog
{
    class MasterFileWriter;
    class AutoLoggerFactory;
    class AutoLogger;
}

namespace logger
{

class MainThread: public hydroiceutil::SubsystemThread
{    	
public:
    MainThread( const orcaice::Context& context );
    ~MainThread();

    // from SubsystemThread
    virtual void walk();

private:

    std::vector<orcalog::AutoLoggerFactory*>         logFactories_;
    std::vector<hydrodll::DynamicallyLoadedLibrary*> libraries_;

    // The guy that writes the master file
    std::auto_ptr<orcalog::MasterFileWriter> masterFileWriter_;
    // The guys that write the data files
    std::vector<orcalog::AutoLogger*> autoLoggers_;

    // utilities
    void loadPluginLibraries( const std::string &factoryLibNames );
    void createLogger( const std::string  &interfaceType, 
                       const std::string  &interfaceTypeSuffix,
                       const std::string  &format,
                       const std::string  &filenamePrefix );

    orcaice::Context context_;
};

} // namespace

#endif
