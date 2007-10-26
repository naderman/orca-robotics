/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOG_MASTER_FILE_READER_H
#define ORCA2_ORCALOG_MASTER_FILE_READER_H

#include <IceUtil/Mutex.h>
#include <orcaice/context.h>
#include <orcalog/masterfilebreadcrumbs.h>

namespace orcalog
{

/*!
     @brief Knows how to parse a master file.
*/
class MasterFileReader
{
public:
    MasterFileReader( const std::string& filename, const orcaice::Context& context );
    ~MasterFileReader();

    //! Get info on all logs from the master file
    void getLogs( std::vector<std::string> &filenames,
                  std::vector<std::string> &interfaceTypes,
                  std::vector<std::string> &formats,
                  std::vector<bool>        &enableds );

    //! Rewind to the start of the data log. Calling getData() afterwards will
    //! return the first data point. Returns 0 if data read sucessfully. Returns 
    //! 1 if end of file is reached without finding data.
    int rewindToStart();

    //! Read one line of data log. Returns 0 if data read sucessfully. Returns 
    //! 1 if end of file is reached.
    int getData( int &seconds, int &useconds, int &id, int &index );

    //! Steps through the data entries until the time stamp is equal to or is after
    //! the one specified. For example, data is logged every second and the last replayed
    //! time was t=3. Calling getData(...,5,0) or getData(...,5,1) will return 
    //! the data at time t=5.
    //!
    //! Seeking time earlier than the current time will move one step forward and return. 
    //! For example, calling getData(..,,2,0) at t=3 will return the data at time t=4.
    //!
    //! Seeking time after the end of the log will result in fast forwarding to the end.
    //!
    //! Return values are same as getData.
    int getDataAtOrAfterTime( int &seconds, int &useconds, int &id, int &index, int seekSec, int seekUsec=0 );

private:

    void calcConstituentLogs();

    std::ifstream *file_;

    // Used for recording positions in the file.
    MasterFileBreadCrumbs breadCrumbs_;

    // remember the dir where the master file is located
    // the individual log files MUST be in the same dir.
    std::string dir_;

    // Details of the logs referenced in this master file
    std::vector<std::string> filenames_;
    std::vector<std::string> interfaceTypes_;
    std::vector<std::string> formats_;
    std::vector<bool>        enableds_;

    // If we read right now we'll read the index_'th data item.
    int index_;
    
    orcaice::Context context_;
};

}

#endif
