/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <fstream>
#include "replayer.h"
#include "exceptions.h"

using namespace std;
using namespace orcalog;

Replayer::Replayer( const std::string      &interfaceType, 
                    const std::string      &format,
                    const std::string      &filename,
                    const orcaice::Context & context )
    : context_(context),
      interfaceType_(interfaceType),
      // derived classes will set their own names
      interfaceName_("unknown"),
      format_(format),
      filename_(filename),
      file_(0),
      dataCounter_(-1)
{    
    //
    // open log file. Derived classes may re-implement this function to modify
    // the standard treatment of log formats.
    //
    // benu: can't call this here as the base class constructor is called first
    // which implements the base class openLogFile() and not the derived class
    // openLogFile()
    // openLogFile();
}

Replayer::~Replayer()
{    
    if ( file_ ) {
        file_->close();
        delete file_;
    }
}
    
void 
Replayer::init( bool isTest )
{
    // instead of re-opening, we should rewind to top.
    // benu: uncommented the openLogFile() here as the 
    // openLogFile() from derived classes will called rather
    // than when called in the constructor
    openLogFile();

    if ( !isTest ) {
        // this one is implemented in derived classes
        initInterfaces();
    
        // this one is implemented in derived classes
        initDescription();
    }
}

void
Replayer::openAsciiFile()
{
    if ( file_ ) {
        context_.tracer()->warning( "Log file already open. Closing and reopening." );
        file_->close();
        delete file_;
        file_ = 0;
    }

    file_ = new std::ifstream( filename_.c_str() );
    if ( !file_->is_open() )  {
        std::string s = "Could not create log file " + filename_;
        context_.tracer()->warning( s );
        throw FileException( ERROR_INFO, s );
    }
}

void
Replayer::openBinaryFile()
{
    if ( file_ ) {
        context_.tracer()->warning( "Log file already open. Closing and reopening." );
        file_->close();
        delete file_;
        file_ = 0;
    }

    file_ = new std::ifstream( filename_.c_str(), ios::binary|ios::in );

    if ( !file_->is_open() )  {
        std::string s = "Could not open log file " + filename_;
        context_.tracer()->warning( s );
        throw FileException( ERROR_INFO, s );
    }
}

void
Replayer::openLogFile()
{
    context_.tracer()->debug( "Opening log file "+filename_,2 );

    //
    // open log file, may throw and it will kill us
    //
    if ( format_ == "ice" ) {
        openBinaryFile();
    }
    else if ( format_ == "ascii" ) {
        openAsciiFile();
    }
}

void 
Replayer::checkFile()
{
    if ( file_->bad() ) {
        stringstream ss;
        ss << "Could not load log file "<< filename_;
        throw orcalog::FileException( ERROR_INFO, ss.str() );
    }
}

void 
Replayer::checkIndex( int index )
{
    if ( index < (dataCounter_+1) ) {
        stringstream ss;
        ss << "Attempt to read the log backwards: last="<<dataCounter_<<" this="<<index;
        throw orcalog::Exception( ERROR_INFO, ss.str() );
    }

    // Let the user know that something's happening
    if ( ! (dataCounter_ % 50 ) )
    {
        ostringstream ss;
        ss << interfaceType_ << "Replayer: replayed " << dataCounter_ << " data objects from "<<filename_;
        context_.tracer()->info( ss.str() );
    } 
}
