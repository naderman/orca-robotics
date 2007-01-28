/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LOG_ICE
#define ORCA2_LOG_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_log Log
    @brief Monitors and controls creation of log files.

Remotely connect to the logging component to check and control logging progress.

    @{
*/

//! 
enum LogState
{
    LogStarted,
    LogStopped,
    LogPaused
};

/*!
    @brief Information about the status of the logging process.

All data is meaningless if the current log state is LogStopped.
*/
struct LogData
{
    //! Time when data was collected.
    Time timeStamp;

    //! The state of the logger.
    LogState state;

    //! Amount of data logged so far [bytes]
    int logSize;

    //! Average log data rate (from the start of this log) [bytes/s]
    int logRate;

    //! Number of interfaces being logged.
    int logCount; 

    //! Amount of space left on the storage device [bytes]
    int spaceLeft;

    //! Estimate of time logging can continue at the current rate.
    //! (May or may not be the same as spaceLeft/logRage)
    Time timeLeft;
};

//! Interface to a device with a binary state.
interface Log
{
    //! Returns log status
    nonmutating LogData getData()
        throws DataNotExistException;

    //! Starts new data set.
    //! If a data set is already started, does nothing.
    idempotent void start();

    //! Stops current data set.
    //! If no data set exists, does nothing.
    idempotent void stop();
    
    //! Temporarily stops logging data.
    //! If data is already paused, does nothing.
    idempotent void pause();

    //! Starts logging data into current data set.
    //! If data is not paused, does nothing.
    idempotent void unpause();
};

/*! @} */
}; // module

#endif
