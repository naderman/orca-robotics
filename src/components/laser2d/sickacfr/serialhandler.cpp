/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "serialhandler.h"
#include <iostream>
#include <orcaice/orcaice.h>

using namespace std;

namespace sickacfr {

namespace {
    const int BUFFER_SIZE_WARN_THRESHOLD = 20000;

    const char *SUBSYSTEM = "SerialHandler";

    // Clear out the bytes we've just parsed, and shift the un-parsed data
    // to the front of the buffer.
    void removeParsedData( std::vector<uChar> &buffer,
                           int numBytesParsed )
    {
        if ( numBytesParsed > 0 )
        {
            int numBytesBeyond = buffer.size()-numBytesParsed;
            if ( numBytesBeyond > 0 )
            {
                memmove( &(buffer[0]), &(buffer[numBytesParsed]), numBytesBeyond*sizeof(uChar) );
            }
            buffer.resize( numBytesBeyond );
        }
    }

}

//////////////////////////////////////////////////////////////////////

SerialHandler::SerialHandler( const std::string &dev,
                              const orcaice::Context &context )
    : serial_(dev.c_str(), 9600, true),
      responseBuffer_(-1,orcaiceutil::BufferTypeCircular),
      isMessageWaitingToBeSent_(false),
      baudRateChangePending_(false),
      context_(context)
{
    const int timeoutSeconds = 0;
    const int timeoutUSeconds = 200000;
    serial_.setTimeout( timeoutSeconds, timeoutUSeconds );

    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 60 );
    context_.status()->initialising( SUBSYSTEM );
}

SerialHandler::~SerialHandler()
{
    //
    // The component may outlive this subsystem.
    // So tell status that it might not hear from us for a while.
    //
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 1e9 );

    //
    // Turn continuous mode off on shutdown
    //

    // Catch all exceptions here -- don't wanna be throwing exceptions from destructor!
    try {
        context_.tracer()->debug( "SerialHandler: Turning continuous mode off" );
        std::vector<uChar> commandAndData;
        constructRequestMeasuredOnRequestMode( commandAndData );
        std::vector<uChar> telegram;
        constructTelegram( telegram, commandAndData );
        serial_.write( &(telegram[0]), telegram.size() );    
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "SerialHandler: Caught exception in destructor: " << e.what();
        context_.tracer()->debug( ss.str() );
    }
    catch ( ... )
    {
        stringstream ss;
        ss << "SerialHandler: Caught unknown  exception in destructor.";
        context_.tracer()->debug( ss.str() );
    }
}

void
SerialHandler::setBaudRate( int baudRate )
{
    stringstream ss; ss << "SerialHandler: baud rate change requested: " << baudRate;
    context_.tracer()->debug( ss.str() );

    IceUtil::Mutex::Lock lock(mutex_);

    assert( !baudRateChangePending_ );

    baudRateChangePending_ = true;
    newBaudRate_ = baudRate;
}

void
SerialHandler::send( const std::vector<uChar> &telegram )
{
    IceUtil::Mutex::Lock lock(mutex_);
    
    assert( !isMessageWaitingToBeSent_ );
    isMessageWaitingToBeSent_ = true;
    toSend_ = telegram;
}

void
SerialHandler::run()
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 2 );

    while ( !isStopping() )
    {
        try {

            // Check for house-keeping jobs first
            {
                IceUtil::Mutex::Lock lock(mutex_);

                if ( baudRateChangePending_ )
                {
                    context_.tracer()->debug( "SerialHandler: Changing baud rate and flushing." );
                    baudRateChangePending_ = false;
                    serial_.setBaudRate( newBaudRate_ );
                    serial_.flush();
                    serial_.drain();
                }

                if ( isMessageWaitingToBeSent_ )
                {
                    stringstream ss;
                    ss<<"SerialHandler: sending: " << toHexString(toSend_);
                    context_.tracer()->debug( ss.str() );

                    isMessageWaitingToBeSent_ = false;
                    serial_.write( &(toSend_[0]), toSend_.size() );
                }
            }

            // Wait for data to arrive, put it in our buffer_
            if ( getDataFromSerial() )
            {
                // Process it
                processBuffer( orcaice::getNow() );
            }
        }
        catch ( std::exception &e )
        {
            stringstream ss;
            ss << "SerialHandler: Caught exception: " << e.what();
            context_.tracer()->warning( ss.str() );
        }
        catch ( ... )
        {
            cout<<"TRACE(serialhandler.cpp): Caught unknown exception." << endl;
        }
    }
}

bool
SerialHandler::getDataFromSerial()
{
    int nBytes = serial_.bytesAvailableWait();

    if ( nBytes > 0 )
    {
        // Resize buffer to make room for new data
        buffer_.resize( buffer_.size() + nBytes );
        int readStart = buffer_.size()-nBytes;
        int numRead = serial_.read( &(buffer_[readStart]), nBytes );
        assert( (numRead == nBytes) && "serial_.read should read exactly the number we ask for." );
        
        if ( (int)(buffer_.size()) > BUFFER_SIZE_WARN_THRESHOLD )
        {
            stringstream ss;
            ss << "SerialHandler:: Buffer is getting pretty big -- size is " << buffer_.size();
            context_.tracer()->warning( ss.str() );
        }

        return true;
    }
    return false;
}

void
SerialHandler::processBuffer( const orca::Time &timeStamp )
{
//     stringstream ssDebug;
//     ssDebug << "SerialHandler::processSerialBuffer: buffer is: " << toHexString(buffer_);
//     context_.tracer()->debug( ssDebug.str() );

    // This loop is in case multiple messages arrived.
    while ( true )
    {
        LmsResponse response;
        int numBytesParsed;
        bool gotMessage = parseBuffer( &(buffer_[0]),
                                       buffer_.size(),
                                       response,
                                       numBytesParsed );

        removeParsedData( buffer_, numBytesParsed );

        if ( gotMessage )
        {
            responseBuffer_.push( TimedLmsResponse( timeStamp, response ) );

            if ( response.isError() )
            {
                stringstream ss;
                ss << "SerialHandler: Received errorred response: " << toString(response);
                context_.tracer()->debug( ss.str() );
            }
        }
        else
        {
            break;
        }
    }
    context_.status()->ok( SUBSYSTEM );
}

}
