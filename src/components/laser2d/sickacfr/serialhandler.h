/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef SICK_ACFR_SERIALHANDLER_H
#define SICK_ACFR_SERIALHANDLER_H

#include <orcaice/thread.h>
#include <sickacfr/messages.h>
#include <orcaserial/serial.h>
#include <orcaice/context.h>
#include <orcaice/buffer.h>
#include <orca/orca.h>

namespace sickacfr {

// LmsResponse plus a timeStamp
class TimedLmsResponse {
public:
    TimedLmsResponse() {}
    TimedLmsResponse( const orca::Time &t, const LmsResponse &r )
        : timeStamp(t), response(r) {}

    orca::Time  timeStamp;
    LmsResponse response;
};

//
// @brief Handles the serial port.
//
// Read in this separate loop so we can hopefully grab the messages
// as soon as they arrive, without relying on having Driver::read()
// called by an external thread which may be doing other stuff.
// This will hopefully give us more accurate timestamps.
//
// @author Alex Brooks
//
class SerialHandler : public orcaice::Thread
{

public: 

    SerialHandler( const std::string &dev,
                   const orcaice::Context &context );
    ~SerialHandler();

    void send( const std::vector<uChar> &telegram );

    void setBaudRate( int baudRate );

    virtual void run();

    // Allow external non-const access direct to (thread-safe) responseBuffer
    orcaice::Buffer<TimedLmsResponse> &responseBuffer() { return responseBuffer_; }

private: 

    // Returns: true if got data, false if timed out
    bool getDataFromSerial();
    void processBuffer( const orca::Time &timeStamp );

    IceUtil::Mutex   mutex_;

    orcaserial::Serial serial_;

    // Contains un-parsed data from the SICK.
    std::vector<uChar> buffer_;

    // Thread-safe store of responses from the SICK
    orcaice::Buffer<TimedLmsResponse> responseBuffer_;

    // Stuff waiting to be sent
    bool isMessageWaitingToBeSent_;
    std::vector<uChar> toSend_;

    bool baudRateChangePending_;
    int newBaudRate_;

    orcaice::Context context_;
};

}

#endif
