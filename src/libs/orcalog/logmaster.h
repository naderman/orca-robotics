/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOG_LOG_MASTER_H
#define ORCA2_ORCALOG_LOG_MASTER_H

#include <IceUtil/Mutex.h>
#include <orcaice/context.h>

namespace orcalog
{

class Logger;

/*!
    All public operations are thread safe.
*/
class LogMaster
{
public:
    LogMaster( const std::string& filename, const orcaice::Context& context );
    ~LogMaster();

    //! Initializes loggers and starts logging data.
    void start();

    //! Stops logging and closes all log files.
    void stop();

    //! Temporariliy stops logging data without closing the data set.
    void pause();

    //! Resumes logging data.
    void unpause();

    //! Returns number of registered loggers.
    int loggerCount() const;

    //! Loggers register themselves.
    //! Returns a unique ID used by loggers later to identify themselves.
    //! Raises orcalog::Exception if the logger cannot be registered, e.g.
    //! because logging has already started.
    int addLog( Logger* logger,
                const std::string& filename,
                const std::string& interfaceType,
                const std::string& encodingType,
                const std::string& proxyString );

    //! Call this to write another data object.
    void addData( int id, int index );

private:
    std::ofstream* file_;
    std::vector<Logger*> loggers_;

    // use this to assign unique ID's to loggers.
    int logCounter_;
    // use this for feedback at the end of the run.
    int dataCounter_;

    bool isStarted_;

    orcaice::Context context_;

    IceUtil::Mutex mutex_;
};

}

#endif
