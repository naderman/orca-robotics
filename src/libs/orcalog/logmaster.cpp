/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <IceUtil/Time.h>
#include <orcaice/orcaice.h>
#include <orcalog/exceptions.h>
#include "logmaster.h"
#include "logger.h"
#include "utils.h"

using namespace std;
using namespace orcalog;


LogMaster::LogMaster( const std::string &filename, const orcaice::Context & context )
    : logCounter_(-1),
      dataCounter_(0),
      isStarted_(false),
      context_(context)
{
    // create master file
    file_ = new ofstream( filename.c_str() );
    if ( !file_->is_open() ) {
        context_.tracer()->warning( "Could not create master file " + filename );
        throw orcalog::FileException( ERROR_INFO, "Could not create master file " + filename );
    }
    
    // write some global information as comments to the header
    (*file_) << "# == Component ==" << endl;
    (*file_) << "# Logger host    : " << orcaice::getHostname() << endl;
    (*file_) << "# Log start time : " << IceUtil::Time::now().toString() << endl;
    (*file_) << "# Ice version    : " << ICE_STRING_VERSION << endl;
    (*file_) << "# Orca version   : " << orcaice::orcaVersion() << endl;
    (*file_) << "# " << endl;
    (*file_) << "# == Interfaces / Logs Legend ==" << endl;
    (*file_) << "# [interface name]" << endl;
    (*file_) << "# [log filename] [interface type] [log format] [replay enabled]" << endl;
    (*file_) << "# " << endl;
    (*file_) << "# == Interfaces / Logs ==" << endl;
}

LogMaster::~LogMaster()
{
    std::stringstream ss;
    ss << "shutting down after writing "<<dataCounter_<<" objects to "<<(logCounter_+1)<<" logs.";
    context_.tracer()->info( ss.str() );

    if ( file_ ) {
        file_->close();
        delete file_;
    }

    // important: do not delete loggers because most of them derive from 
    // Ice smart pointers and self-destruct. Deleting them here will result
    // in seg fault.
}

void 
LogMaster::start()
{
    for ( unsigned int i=0; i<loggers_.size(); i++)
    {
        loggers_[i]->init();
    }
}

void 
LogMaster::stop()
{
}

void 
LogMaster::pause()
{
}

void 
LogMaster::unpause()
{
}

int 
LogMaster::loggerCount() const
{
    IceUtil::Mutex::Lock lock(mutex_);

    return loggers_.size();
}

int
LogMaster::addLog( Logger* logger,
                    const std::string & filename, 
                    const std::string & interfaceType,
                    const std::string & format,
                    const std::string & proxyString )
{
    assert( logger && "zero logger pointer" );

    context_.tracer()->debug( 
        "adding log: file="+filename+" id="+interfaceType+" fmt="+format+" prx="+proxyString, 5 );

    IceUtil::Mutex::Lock lock(mutex_);

    if ( isStarted_ ) {
        context_.tracer()->warning("Can't register after started appending!");
        throw orcalog::Exception( ERROR_INFO, "Can't register after started appending!" );
    }

    // keep pointer to control this logger in the future
    loggers_.push_back( logger );

    // this is just a comment 
    (*file_) << "# " << proxyString << endl;

    // this will be used replay this log back, we enable play back by default
    bool enabled = true;
    (*file_) << orcalog::headerLine( filename, interfaceType, format, enabled ) << endl;

    // increment unique logger id
    ++logCounter_;

    return logCounter_;
}

void 
LogMaster::addData( int id, int index )
{
    IceUtil::Mutex::Lock lock(mutex_);

    if ( !isStarted_ ) {
        // Write the beginning of data field (end of the header)
        (*file_) << "# " << endl;
        (*file_) << "# == Data Legend ==" << endl;
        (*file_) << "# seconds useconds file_id object_id" << endl;
        (*file_) << "# " << endl;
        (*file_) << orcalog::endOfHeader() << endl;
        isStarted_ = true;
    }

    //
    // write to master file
    //
    orca::Time now = orcaice::getNow();
    (*file_) << orcalog::dataLine( now.seconds, now.useconds, id, index ) << endl;

    // count data objects
    ++dataCounter_;
}
