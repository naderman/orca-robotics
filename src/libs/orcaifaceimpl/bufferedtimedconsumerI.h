/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_BUFFERED_TIMED_CONSUMER_H
#define ORCAIFACEIMPL_BUFFERED_TIMED_CONSUMER_H

#include <orcaifaceimpl/bufferedconsumerI.h>
#include <orcaice/timer.h>

namespace orcaifaceimpl
{

//!
//! @brief Useful for receiving data from a stream, sticking it in a buffer, and noting the time
//! since an object was last received.
//!
//! @author Alex Brooks
//!
template< class ConsumerType, class DataType >
class BufferedTimedConsumerI : public orcaifaceimpl::BufferedConsumerI<ConsumerType,DataType>
{
public:

    double msSinceReceipt() { return timer_.elapsedMs(); }

    //! An immediately subsequent call to 'msSinceReceipt' will return zero.
    void resetTimer() { return timer_.restart(); }

private:
    virtual void handleData( const DataType & data )
    {
        timer_.restart();
    };

    orcaice::Timer  timer_;
};

} // namespace

#endif