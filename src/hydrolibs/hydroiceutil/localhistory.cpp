/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <fstream>
#include <IceUtil/Time.h>
#include <hydroutil/properties.h>
#include <hydroutil/stringutils.h>
#include <hydroutil/sysutils.h>
#include <gbxutilacfr/exceptions.h>
#include <gbxutilacfr/mathdefs.h>
#include <hydroiceutil/timeutils.h>

#include "localhistory.h"

using namespace std;
using namespace hydroiceutil;

LocalHistory::LocalHistory( const hydroutil::Properties& props ) :
    lineWidth_(0),
    properties_(props)
{
//     cout<<"LocalHistory: initializing with properties:"<<endl
//         <<properties_.toString()<<endl;

    enabled_ = properties_.getPropertyAsIntWithDefault( "Enable", 0 );

    if ( !enabled_ )
        return;

    autoSaveInterval_ = properties_.getPropertyAsIntWithDefault( "AutoSaveInterval", 60 );
    assert( autoSaveInterval_!=0 && "AutoSaveInterval can be set to zero" );

    string path;
    if ( properties_.getProperty( "Path", path ) )
    {
        // if "Path" is not specified, try to construct it from other props.
        string dir = properties_.getPropertyWithDefault( "Dir", "." );
        string filename = properties_.getPropertyWithDefault( "DefaultFilename", "history.txt" );
        path = dir + "/" + filename;
    }

    file_.reset( new fstream );
    // we need to open for both reading and writing (not sure why)
    // we want to add contents to the file but not always to the end
    // see http://stdcxx.apache.org/doc/stdlibug/30-3.html
    // but read/write access cannot be granted if the file does not exist. thank you STL.
    if ( hydroutil::fileExists( path ) )
        file_->open( path.c_str(), ios::in | ios::out | ios_base::ate );
    else
        file_->open( path.c_str(), ios::out );

    if ( !file_->is_open() ) {
        throw gbxutilacfr::Exception( ERROR_INFO, "LocalHistory: Could not create file "+path );
    }
    else {
        cout<<"hydro::LocalHistory: Opened history file " << path << endl;
    }

    // this can be used instead of specifying ::ate
//     file_->seekp( 0, std::ios::end );

    // remember starting write position in the stream
    startPos_ = file_->tellp();
    cout<<"star pos="<<startPos_<<endl;

    // write the first standard progress message
    flush();
}

LocalHistory::~LocalHistory()
{
    if ( !enabled_ )
        return;

    IceUtil::Mutex::Lock lock(mutex_);

    // write the last progress report
    flush();
}

void
LocalHistory::report( const std::string &newMessage )
{
    if ( !enabled_ )
        return;

    IceUtil::Mutex::Lock lock(mutex_);

    // ignore empty messages
    if ( !newMessage.empty() )
        message_ = newMessage;

    maybeFlush();
}

void
LocalHistory::flush()
{
    stringstream ss;
    ss << IceUtil::Time::now().toDateTime() << " " << runTimer_.elapsedSec() << " " << message_;
    string report = ss.str();

    // remember our line width
    lineWidth_ = MAX( lineWidth_, (int)report.size() );

    const bool adjustLeft = true;
    report = hydroutil::toFixedWidth( report, lineWidth_, ' ', adjustLeft );

    // rewind to the stored location
    file_->seekp( startPos_ );
    (*file_.get()) << report << endl;

    flushTimer_.restart();
}

void
LocalHistory::maybeFlush()
{
    if ( autoSaveInterval_>0 && flushTimer_.elapsedSec() > autoSaveInterval_ )
        flush();
}
