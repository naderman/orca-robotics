/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <orcaice/orcaice.h>
#include <orcalog/exceptions.h>

#include "masterfilereader.h"
#include "detail/filebreadcrumbs.h"
#include "utils.h"

using namespace std;
using namespace orcalog;


MasterFileReader::MasterFileReader( const std::string &filename, const orcaice::Context& context )
    : cursorValid_(false),
      context_(context),
      breadCrumbs_(0)
{
    // create master file
    file_ = new ifstream( filename.c_str() );
    if ( !file_->is_open() ) {
        throw orcalog::FileException( ERROR_INFO, "Could not open master file " + filename );
    }
    breadCrumbs_ = new detail::FileBreadCrumbs<IceUtil::Time>();

    // remember the dir where the master file is located
    // the individual log files MUST be in the same dir.
    dir_ = hydroutil::dirname( filename );

    calcConstituentLogs();

    calcCursorInfo();
    if ( !cursorValid_ )
    {
        throw hydroutil::Exception( ERROR_INFO, "Initial calcCursorInfo failed.  Is there any data in this log?" );
    }

    initialCursorTime_ = iceUtilTime(cursorSec_,cursorUsec_);
    rewindToStart();
}

MasterFileReader::~MasterFileReader()
{
    if ( file_ ) {
        file_->close();
        delete file_;
    }
    delete breadCrumbs_;
}

void
MasterFileReader::calcConstituentLogs()
{
    std::string line;
    std::string filename;
    std::string interfaceType;
    std::string format;
    bool enabled;

    while ( std::getline( *file_, line ) ) {
        // data starts here, don't parse headers anymore
        if ( orcalog::isEndOfHeader( line ) ) {
            break;
        }
    
        // skip comments (check for it AFTER the end-of-header in case it starts with #)
        if ( line[0] == '#' ) {
            continue;
        }
        
        orcalog::parseHeaderLine( line, filename, interfaceType, format, enabled );

        // add full path to the file name
        filename = dir_ + hydroutil::pathDelimeter() + filename;

        filenames_.push_back( filename );
        interfaceTypes_.push_back( interfaceType );
        formats_.push_back( format );
        enableds_.push_back( enabled );

        context_.tracer()->debug( "MasterFileReader: Parsed header: file="+filename+" type="+interfaceType+" fmt="+format, 5);
    }
}

void
MasterFileReader::getLogs( std::vector<std::string>& filenames,
                           std::vector<std::string>& interfaceTypes,
                           std::vector<std::string>& formats,
                           std::vector<bool>& enableds )
{
    filenames      = filenames_;
    interfaceTypes = interfaceTypes_;
    formats        = formats_;
    enableds       = enableds_;
}

void 
MasterFileReader::rewindToStart()
{   
    std::ios::pos_type crumbPos;
    detail::SeekResult res = breadCrumbs_->getCrumbAtOrAfter( iceUtilTime(0,0), crumbPos );
    assert( (res == detail::SeekOK) && "Should be able to find first crumb!" );

    moveTo( crumbPos );
    assert( cursorValid_ );
}

void
MasterFileReader::calcCursorInfo()
{
    int ret = readData( cursorSec_, cursorUsec_, cursorId_, cursorIndex_ );
    cursorValid_ = (ret==0);
}

int 
MasterFileReader::readData( int& seconds, int& useconds, int& id, int& index )
{
    std::string line;

    while ( !file_->eof() ) 
    {
        // Remember the position of the start of this line.
        std::ios::pos_type lineStartPos = file_->tellg();

        std::getline( *file_, line );

        // skip empty lines and comments
        if ( line.empty() || line[0] == '#' ) {
            continue;
        }
        
        // this may throw when parsing fails, we just print a warning and go to the next line
        try
        {
            orcalog::parseDataLine( line, seconds, useconds, id, index );
        }
        catch ( const orcalog::Exception& e )
        {
            context_.tracer()->warning( "parsing error in master file line: '" + line + "'" );
            continue;
        }

        // success
        breadCrumbs_->placeCrumb( lineStartPos, iceUtilTime(seconds, useconds) );
        return 0;
    }
    
    return 1;
}

int 
MasterFileReader::getData( int& seconds, int& useconds, int& id, int& index )
{
    if ( cursorValid_ )
    {
        seconds  = cursorSec_;
        useconds = cursorUsec_;
        id       = cursorId_;
        index    = cursorIndex_;

        calcCursorInfo();
        return 0;
    }
    else
    {
        return 1;
    }
}

void
MasterFileReader::placeCursorBeforeTime( int seekSec, int seekUsec )
{
    int seconds, useconds, id, index;
    // don't care if we read to the end of the file.
    getDataAtOrAfterTime( seconds, useconds, id, index, seekSec, seekUsec );

    // Now we know that the breadCrumbs_ will have passed the specified point.
    // Get what we want from the breadCrumbs_.

    std::ios::pos_type crumbPos;
    detail::SeekResult result = breadCrumbs_->getCrumbBefore( iceUtilTime(seekSec, seekUsec),
                                                             crumbPos );

    if ( result == detail::SeekOK )
    {
        moveTo( crumbPos );
    }
    else if ( result == detail::SeekQueryBeforeStart )
    {
        rewindToStart();
    }
    else
    {
        assert( false && "unexpected result" );
    }
}

void
MasterFileReader::stepBackward()
{
    if ( !cursorValid_ )
    {
        // cursorSec_:cursorUsec_ is the time of the last item.
        // Point at it.
        placeCursorAtOrAfterTime( cursorSec_, cursorUsec_ );
    }
    else
    {
        // Point at the item before the current one.
        placeCursorBeforeTime( cursorSec_, cursorUsec_ );
    }
}

int 
MasterFileReader::placeCursorAtOrAfterTime( int seekSec,
                                            int seekUsec )
{
    // Maybe the requested time is in the past, somewhere in the trail of breadcrumbs.
    std::ios::pos_type crumbPos;
    detail::SeekResult result = breadCrumbs_->getCrumbAtOrAfter( iceUtilTime(seekSec, seekUsec), 
                                                                crumbPos );

    if ( result == detail::SeekOK )
    {
        // Found it
        moveTo( crumbPos );
        return 0;
    }
    else if ( result == detail::SeekQueryInFuture )
    {
        // The piece of data we want is some time into the future.  Have to search forwards.
        int seconds, useconds, id, index;
        IceUtil::Time seekTime = iceUtilTime( seekSec, seekUsec );
        while ( !getData( seconds, useconds, id, index ) ) 
        {
            if ( iceUtilTime(seconds,useconds) >= seekTime ) 
            {
                // Found what we want.  Now step back one so the cursor is pointing
                // at what was just found.
                stepBackward();
                return 0;
            }
        }
        // eof
        return 1;
    }
    else
    {
        assert( false && "unknown SeekResult." );
        return 1;
    }
}

int 
MasterFileReader::getDataAtOrAfterTime( int& seconds,
                                        int& useconds,
                                        int& id,
                                        int& index,
                                        int seekSec,
                                        int seekUsec )
{
    int ret = placeCursorAtOrAfterTime( seekSec, seekUsec );
    if ( ret != 0 )
    {
        return ret;
    }
    else
    {
        getData( seconds, useconds, id, index );
        return 0;
    }
}

bool
MasterFileReader::getCursorTime( int &seconds,
                                 int &useconds )
{
    if ( !cursorValid_ )
        return false;

    seconds = cursorSec_;
    useconds = cursorUsec_;
    return true;
}

void
MasterFileReader::moveTo( const std::ios::pos_type &pos )
{
    // Clear eof bit
    file_->clear();
    file_->seekg( pos );
    calcCursorInfo();
    
}
