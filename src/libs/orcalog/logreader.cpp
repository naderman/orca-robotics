/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <fstream>
#include <hydroutil/hydroutil.h>

#include "logreader.h"
#include "exceptions.h"
#include "detail/filebreadcrumbs.h"

using namespace std;

namespace orcalog {

LogReader::LogReader( const LogReaderInfo &logReaderInfo )
    : file_(0),
      logReaderInfo_(logReaderInfo),
      logIndex_(-1),
      breadCrumbs_(0)
{
    breadCrumbs_.reset( new detail::FileBreadCrumbs<int> );
}

LogReader::~LogReader()
{    
    if ( file_ ) {
        file_->close();
        delete file_;
    }
}
    
void 
LogReader::init()
{
    //
    // create log file. Derived classes may re-implement this function to modify
    // the standard treatment of log formats.
    //
    // Note: openLogFile() has to be called from here, because
    // calling it from the constructor would call the base-class's version.
    //
    openLogFile();
}

void
LogReader::openLogFile()
{
    logReaderInfo_.context.tracer().debug( "Opening log file "+logReaderInfo_.filename,2 );

    if ( file_ ) {
        std::string s = "Log file already exists.";
        throw FileException( ERROR_INFO, s );
    }

    //
    // open log file, may throw and it will kill us
    //
    if ( logReaderInfo_.format == "ice" ) {
        file_ = openBinaryFileForReading( logReaderInfo_.filename );
    }
    else if ( logReaderInfo_.format == "asciigenerated" ) {
        file_ = openAsciiFileForReading( logReaderInfo_.filename );
    }
    else if ( logReaderInfo_.format == "ascii" ) {
        file_ = openAsciiFileForReading( logReaderInfo_.filename );
    }
    else {
        assert( false && "base class can only handle 'ice' and 'ascii' and 'asciigenerated' formats" );
    }
}

void
LogReader::setLogIndex( int index )
{
    if ( index == logIndex_ )
    {
        // The usual case when we're moving through sequentially
        return;
    }

    if ( index > breadCrumbs_->latest() )
    {
        // Have to fast-forward into the future
        while ( logIndex_ < index )
        {
            read();
        }
    }
    else // index < breadCrumbs_->latest()
    {
        // Have to rewind into the past
        std::ios::pos_type crumbPos;
        bool found = breadCrumbs_->getCrumbAt( index, crumbPos );
        if ( !found )
        {
            stringstream ss;
            ss << "Failed to find crumb in past.  index="<<index<<", logIndex_="<<logIndex_;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
        file_->seekg( crumbPos );
    }
    logIndex_ = index;
}

void 
LogReader::zeroLogIndex()
{
    logIndex_ = 0;
    breadCrumbs_->placeCrumb( file_->tellg(), logIndex_ );
}

void 
LogReader::advanceLogIndex()
{
    logIndex_++;
    breadCrumbs_->placeCrumb( file_->tellg(), logIndex_ );

    // Let the user know that something's happening
    if ( (logIndex_ % 50)==0 )
    {
        ostringstream ss;
        ss << logReaderInfo_.interfaceType << "LogReader: replayed " << logIndex_ << " data objects from "<<logReaderInfo_.filename;
        logReaderInfo_.context.tracer().info( ss.str() );
    } 
}

std::string
LogReader::toString() const
{
    return logReaderInfo_.interfaceName;
}

//////////////////////////////////////////////////////////////////////

std::ifstream *
openAsciiFileForReading( const std::string &filename )
{
    std::ifstream *file = new std::ifstream( filename.c_str() );
    if ( !file->is_open() )  {
        std::string s = "Could not create log file " + filename;
        throw FileException( ERROR_INFO, s );
    }
    return file;
}

std::ifstream *
openBinaryFileForReading( const std::string &filename )
{
    std::ifstream *file = new std::ifstream( filename.c_str(), ios::binary|ios::in );

    if ( !file->is_open() )  {
        std::string s = "Could not open log file " + filename;
        throw FileException( ERROR_INFO, s );
    }
    return file;
}

}
