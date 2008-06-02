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
#include <orcalog/orcalog.h>
#include <hydrodll/dll.h>
#include <string>
#include <memory>
#include <orcaifaceimpl/buttonImpl.h>

namespace snapshotlogger
{

class MainThread: public orcaice::SubsystemThread,
                  public gbxsickacfr::gbxiceutilacfr::NotifyHandler<bool>
{    	
public:
    MainThread( const orcaice::Context& context );
    ~MainThread();

    // from SubsystemThread
    virtual void walk();

    // from gbxsickacfr::gbxiceutilacfr::NotifyHandler -- called on re-init request
    // (the value of the bool is meaningless)
    virtual void handleData( const bool &request )
        { requestStore_.set( request ); }

private:

    void initLoggers();
    void initInterface();
    void loadPluginLibraries( const std::string &factoryLibNames );
    // throws exceptions if it can't create the logger
    orcalog::SnapshotLogger *createLogger( const std::string &interfaceType );
    std::string filenamePrefix();
    orcalog::MasterFileWriter *createMasterFileWriter( const std::string &filenamePrefix );

    void takeSnapshot();

    std::vector<orcalog::SnapshotLoggerFactory*>     logFactories_;
    std::vector<hydrodll::DynamicallyLoadedLibrary*> libraries_;

    // The guys that write the data files
    std::vector<orcalog::SnapshotLogger*> snapshotLoggers_;
    // Information about them
    std::vector<orcalog::LogWriterInfo> logWriterInfos_;

    // interface to handle requests to take a snapshot
    orcaifaceimpl::ButtonImplPtr buttonInterface_;

    // Holds incoming requests to take snapshots
    // (the value of the bool's are meaningless)
    gbxsickacfr::gbxiceutilacfr::Store<bool> requestStore_;

    bool useFilenameTimestamps_;

    orcaice::Context context_;
};

} // namespace

#endif
