/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>

#include <hydrodll/dll.h>
#include <string>
// #include <memory>

namespace orcalog
{
    class MasterFileWriter;
    class AutoLoggerFactory;
    class AutoLogger;
}

namespace logrecorder
{

class MainThread: public orcaice::SubsystemThread
{    	
public:
    MainThread( const orcaice::Context& context );
    ~MainThread();

private:
    // from SubsystemThread
    virtual void initialise();
    // this subsystem does not do any work!

    void loadPluginLibraries( const std::string &factoryLibNames );
    // throws exceptions if it can't create the logger
    orcalog::AutoLogger *createLogger( const std::string  &interfaceType, const orcaice::Context& context );

    std::vector<orcalog::AutoLoggerFactory*>         logFactories_;
    std::vector<hydrodll::DynamicallyLoadedLibrary*> libraries_;

    // The guy that writes the master file
//     std::auto_ptr<orcalog::MasterFileWriter> masterFileWriter_;
    orcalog::MasterFileWriter* masterFileWriter_;
    // The guys that write the data files
    std::vector<orcalog::AutoLogger*> autoLoggers_;

    orcaice::Context context_;
};

} // namespace

#endif
