/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOG_REPLAY_MASTER_H
#define ORCA2_ORCALOG_REPLAY_MASTER_H

#include <IceUtil/Mutex.h>
#include <orcaice/context.h>

namespace orcalog
{

/*!
    All public operations are thread safe.
*/
class ReplayMaster
{
public:
    ReplayMaster( const std::string & filename, const orcaice::Context & context );
    ~ReplayMaster();

    //! Get info on all logs from the master file
    void getLogs( std::vector<std::string> & filenames,
                    std::vector<std::string> & interfaceTypes,
                    std::vector<std::string> & formats );

    //! Rewind to the beginning of data log.
    void rewindToStart();

    //! Read one line of data log. Returns 0 if data read sucessfully. Returns 
    //! 1 if end of file is reached.
    int getData( int & seconds, int & useconds, int & id, int & index );

private:
    std::ifstream *file_;

    // remember the dir where the master file is located
    // the individual log files MUST be in the same dir.
    std::string dir_;

    orcaice::Context context_;

    IceUtil::Mutex mutex_;
};

}

#endif
