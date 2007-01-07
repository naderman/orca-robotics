/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_PTR_BUFFERED_TIMED_CONSUMER_H
#define ORCAICE_PTR_BUFFERED_TIMED_CONSUMER_H

#include <orcaice/ptrbufferedconsumerI.h>
#include <orcaice/timer.h>

namespace orcaice
{

//!
//! @brief Useful for receiving data from a stream, sticking it in a buffer, and noting the time
//! since an object was last received.
//!
//! @author Alex Brooks
//!
template< class ConsumerType, class DataPtrType >
class PtrBufferedTimedConsumerI : public orcaice::PtrBufferedConsumerI<ConsumerType,DataPtrType>
{
public:

    double msSinceReceipt() { return timer_.elapsedMs(); }

    //! An immediately subsequent call to 'msSinceReceipt' will return zero.
    void resetTimer() { return timer_.restart(); }

private:
    virtual void handleData( const DataPtrType & data )
    {
        timer_.restart();
    };

    orcaice::Timer  timer_;
};

} // namespace

#endif
