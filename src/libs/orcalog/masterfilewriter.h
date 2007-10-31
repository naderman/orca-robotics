/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOG_MASTER_FILE_WRITER_H
#define ORCA2_ORCALOG_MASTER_FILE_WRITER_H

#include <IceUtil/Mutex.h>
#include <orcaice/context.h>

namespace orcalog
{

/*!
    @brief Maintains the master file.

    All public operations are thread safe.
*/
class MasterFileWriter
{
public:
    MasterFileWriter( const std::string &filename,
                      const orcaice::Context &context );
    ~MasterFileWriter();

    //! Returns number of registered loggers.
    int loggerCount() const;

    //! Loggers register themselves.
    //! Returns a unique ID used by loggers later to identify themselves.
    //! Raises orcalog::Exception if the logger cannot be registered, e.g.
    //! because logging has already started.
    int addLog( const std::string &filename,
                const std::string &interfaceType,
                const std::string &encodingType,
                const std::string &comment );

    //! Call this when an item is written in a sub-logfile
    void notifyOfLogfileAddition( int loggerId,
                                  int indexInSubLogfile );

private:

    std::ofstream* file_;

    // use this to assign unique ID's to loggers.
    int numLoggers_;
    // use this for feedback at the end of the run.
    int numItemsLogged_;

    bool isStarted_;

    orcaice::Context context_;

    IceUtil::Mutex mutex_;
};

}

#endif
