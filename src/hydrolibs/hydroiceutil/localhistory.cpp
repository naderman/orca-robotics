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
#include <gbxutilacfr/exceptions.h>
#include <hydroiceutil/timeutils.h>

#include "localhistory.h"

using namespace std;
using namespace hydroiceutil;

LocalHistory::LocalHistory( const hydroutil::Properties& props ) :
    properties_(props)
{
//     cout<<"LocalHistory: initializing with properties:"<<endl
//         <<properties_.toString()<<endl;

    enabled_ = properties_.getPropertyAsIntWithDefault( "Enable", 0 );

    if ( !enabled_ )
        return;

    string path;
    if ( properties_.getProperty( "Path", path ) )
    {
        // if "Path" is not specified, try to construct it from other props.
        string dir = properties_.getPropertyWithDefault( "Dir", "." );
        string filename = properties_.getPropertyWithDefault( "DefaultFilename", "history.txt" );
        path = dir + "/" + filename;
    }

    file_.reset( new ofstream );
    file_->open( path.c_str(), ios_base::out | ios_base::app );

    if ( !file_->is_open() ) {
        throw gbxutilacfr::Exception( ERROR_INFO, "LocalHistory: Could not create file "+path );
    }
    else {
        cout<<"hydro::LocalHistory: Created output file " << path << endl;
    }
}

void
LocalHistory::set( const std::string &message )
{
    if ( !enabled_ )
        return;

    IceUtil::Mutex::Lock lock(mutex_);

    message_ = message;
}

void
LocalHistory::setWithStartSequence( const std::string &message )
{
    if ( !enabled_ )
        return;

    IceUtil::Mutex::Lock lock(mutex_);

    message_ = "1 " + IceUtil::Time::now().toDateTime() + " " + message;
}

void
LocalHistory::setWithFinishSequence( const std::string &message )
{
    if ( !enabled_ )
        return;

    IceUtil::Mutex::Lock lock(mutex_);

    message_ = "0 " + IceUtil::Time::now().toDateTime() + " " + message;
}

void 
LocalHistory::autoStart( bool force )
{
    if ( !enabled_ )
        return;

    IceUtil::Mutex::Lock lock(mutex_);

    if ( message_.empty() )
        message_ = "1 " + IceUtil::Time::now().toDateTime();
}

void 
LocalHistory::autoFinish( bool force )
{
    if ( !enabled_ )
        return;

    IceUtil::Mutex::Lock lock(mutex_);

    if ( message_.empty() )
        message_ = "0 " + IceUtil::Time::now().toDateTime();
}

void
LocalHistory::flush()
{
    if ( !enabled_ )
        return;

    IceUtil::Mutex::Lock lock(mutex_);

    (*file_.get()) << message_ << endl;

    message_.clear();
}
