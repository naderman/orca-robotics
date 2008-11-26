/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCALOG_LOGREADER_H
#define ORCA_ORCALOG_LOGREADER_H

#include <string>
#include <vector>
#include <orcaice/context.h>
#include <orcalog/logreaderinfo.h>

namespace orcalog
{

    namespace detail{
        template<typename IndexType>
        class FileBreadCrumbs;
    }

//! Handles a file which contains actual data (ie not the master file).
//! All LogReaders inherit from this. 
//! Some file handling is done in the base class.
class LogReader
{
public:

    LogReader( const LogReaderInfo &logReaderInfo );
    virtual ~LogReader();

    //! Opens data file (using openLogFile())
    void init();

    const LogReaderInfo &logReaderInfo() const { return logReaderInfo_; }

    //! Handle to the input file.
    std::ifstream *file_;

    //! An index into the log: we're currently pointing at the start of the logIndex_'th object.
    int logIndex() const { return logIndex_; }

    //! Move about in the log.
    void setLogIndex( int index );

    //! Returns a human-readable string.
    //! This is a reasonable default which can be over-ridden.
    virtual std::string toString() const;

protected:

    // Derived classes should call this to let us know when we're at log index zero
    void zeroLogIndex();

    // Derived classes should call this when they have just read an item.
    void advanceLogIndex();

    //! This version correctly handles two standard log file formats:
    //!     ice   ; binary file
    //!     ascii : ascii file
    //! This function does not catch exceptions which may result when opening a file.
    //! Derived classes which want to modify this behavior or need to handle other
    //! formats need to re-implement this function.
    virtual void openLogFile();

    //! Read a log item and chuck it away (base-class has to implement).
    //! Used for fast-forwarding through a file.
    virtual void read()=0;

    //alen - Needed to move this into protected, otherwise openLogFile could not be overwritten
    LogReaderInfo logReaderInfo_;

private:

    // An index into the log: we're currently pointing at the start of the logIndex_'th object.
    int logIndex_;

    // As we move through the file, leave a trail of bread-crumbs so we can rewind
    // (use the index as the key)
    std::auto_ptr< detail::FileBreadCrumbs<int> > breadCrumbs_;
};

//! Optional utility, which allocates space for opened file.
//! Raises FileException if something is wrong.
std::ifstream *openAsciiFileForReading( const std::string &filename );

//! Optional utility, which allocates space for opened file.
//! Raises FileException if something is wrong.
std::ifstream *openBinaryFileForReading( const std::string &filename );

} // namespace

#endif
