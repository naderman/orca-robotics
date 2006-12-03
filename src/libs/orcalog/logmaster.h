/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

/*!
    All public operations are thread safe.
*/
class LogMaster
{
public:
    LogMaster( const std::string & filename, const orcaice::Context & context );
    ~LogMaster();

    //! Loggers register themselves.
    //! Returns a unique ID used by loggers later to identify themselves.
    //! Raises orcalog::Exception if the logger cannot be registered, e.g.
    //! because logging has already started.
    int addLog( const std::string & filename,
                const std::string & interfaceType,
                const std::string & encodingType,
                const std::string & proxyString );

    //! Call this to write another data object.
    void addData( int seconds, int useconds, int id, int index );

private:
    std::ofstream *file_;

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
