/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_REPLAY_ICE
#define ORCA2_REPLAY_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_replay Replay
    @brief Monitors and controls replay of log files.

Remotely connect to the log playing component to check and control replay progress.

    @{
*/

//! States of the log playing process
enum ReplayState
{
    ReplayStarted,
    ReplayStopped,
    ReplayPaused
};

/*!
    @brief Information about the status of the replay process.

All data is meaningless if the current log state is ReplayStopped.
*/
struct ReplayData
{
    //! Time when data was collected.
    Time timeStamp;

    //! The state of the log player.
    ReplayState state;

    //! Number of interfaces being replayed.
    int logCount; 
};

//! Interface to the log playing process.
interface Replay
{
    //! Returns replay status
    nonmutating ReplayData getData()
        throws DataNotExistException;

    //! Starts replaying data set.
    //! If a data set is already started, does nothing.
    idempotent void start();

    //! Stops replaying current data set.
    //! If no data set exists, does nothing.
    idempotent void stop();
    
    //! Temporarily stops replaying data set.
    //! If the process is already paused, does nothing.
    idempotent void pause();

    //! Resumes replaying data from the current data set.
    //! If the process is not paused, does nothing.
    idempotent void unpause();
};

/*! @} */
}; // module

#endif
