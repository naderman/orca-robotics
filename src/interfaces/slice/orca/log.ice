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

/*!
    @brief Information about the status of the logging process.
*/
struct LogData
{
    //! Time when data was collected.
    Time timeStamp;

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
    nonmutating LogData getData()
        throws DataNotExistException;

    //! Starts new data set.
    void startNewLog();
};

/*! @} */
}; // module

#endif
