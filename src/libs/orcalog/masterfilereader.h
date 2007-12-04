/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOG_MASTER_FILE_READER_H
#define ORCA2_ORCALOG_MASTER_FILE_READER_H

#include <IceUtil/Mutex.h>
#include <orcaice/context.h>

namespace orcalog
{
    namespace detail{
        template<typename IndexType>
        class FileBreadCrumbs;
    }

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
    //! return the first data point.
    void rewindToStart();

    //! Read one line of data log. 
    //! Returns:
    //!  - 0 if data read sucessfully.
    //!  - 1 if end of file is reached.
    int getData( int &seconds, int &useconds, int &id, int &index );

    //! Steps through the data entries until the time stamp is equal to or is after
    //! the one specified. For example, data is logged every second and the last replayed
    //! time was t=3. Calling getData(...,5,0) or getData(...,5,1) will return 
    //! the data at time t=5.
    //!
    //! Seeking time after the end of the log will result in fast forwarding to the end.
    //!
    //! Returns:
    //!  - 0 if data read sucessfully.
    //!  - 1 if end of file is reached.
    int getDataAtOrAfterTime( int &seconds, int &useconds, int &id, int &index, int seekSec, int seekUsec=0 );

    //! Places the cursor at the item with the greatest time < seekSec:seekUsec.
    void placeCursorBeforeTime( int seekSec, int seekUsec=0 );

    //! Places the cursor at the item with the smallest time >= seekSec:seekUsec.
    //! Returns:
    //!  - 0 if sucessful.
    //!  - 1 if end of file is reached.
    int placeCursorAtOrAfterTime( int seekSec, int seekUsec=0 );

    //! Gives the time of the item which would be returned if getData() were called.
    //! Returns: cursorIsValid (is invalid at end of log)
    bool getCursorTime( int &seconds, int &useconds );

    //! Steps the cursor backward (unless it's already at the start).
    //! Ie interleaved calls to stepBackward() and getData() will move the cursor back
    //! and forth between two locations.
    void stepBackward();

private:

    void calcConstituentLogs();
    void calcCursorInfo();
    int readData( int& seconds, int& useconds, int& id, int& index );
    void moveTo( const std::ios::pos_type &pos );

    std::ifstream *file_;

    // Used for recording positions in the file.
    detail::FileBreadCrumbs<IceUtil::Time> *breadCrumbs_;

    // The time of the first entry
    IceUtil::Time initialCursorTime_;

    // What's at the cursor.  Ie:
    //   if (cursorValid): what we'll get if we call getData() now.
    //   if (!cursorValid): the last timeStamp in the file.
    int cursorSec_;
    int cursorUsec_;
    int cursorId_;
    int cursorIndex_;

    bool cursorValid_;

    // remember the dir where the master file is located
    // the individual log files MUST be in the same dir.
    std::string dir_;

    // Details of the logs referenced in this master file
    std::vector<std::string> filenames_;
    std::vector<std::string> interfaceTypes_;
    std::vector<std::string> formats_;
    std::vector<bool>        enableds_;

    orcaice::Context context_;
};

}

#endif
