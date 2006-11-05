/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_CPU_ICE
#define ORCA2_CPU_ICE

#include <orca/orca.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_cpu Cpu
    @brief Computer status

    @{
*/

/*!
    @brief CPU information.
*/
class CpuData extends OrcaObject
{
    //! CPU load in user space [%]
    int userLoad;
    //! CPU load in system space [%]
    int systemLoad;
    //! CPU idle load [%]
    int idleLoad;
    //! Temperature of processor or motherboard [C]
    float temperature;
};

//! Interface to CPU information.
interface Cpu
{
    //! Get current CPU state. Raises DataNotExistException if data is not available.
    nonmutating CpuData getData()
        throws DataNotExistException;
};

/*! @} */
}; // module

#endif
