/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PARTICLE2D_INTERFACE_ICE
#define ORCA2_PARTICLE2D_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <IceStorm/IceStorm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_particle2d Particle2d
    @brief Interface for a 2D Particle-based Localiser

    @{
*/

//! A single particle
struct SingleParticle2d
{
    //! pose
    Frame2d pose;
    //! weight
    float   weight;
};

//! A sequence of particles
sequence<SingleParticle2d>  Particle2dSequence;

/*!
    Particle2d data structure
*/
struct Particle2dData
{
    //! particles
    Particle2dSequence particles;
};

/*!
    @brief Consumer of 2d particles
*/
interface Particle2dConsumer
{
    //! set data
    void setData( Particle2dData obj );
};

/*!
    @brief Interface for a 2d particle-based localiser.
 */
interface Particle2d
{
    //! Returns the latest data.
    idempotent Particle2dData getData()
            throws DataNotExistException;

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( Particle2dConsumer* subscriber )
        throws SubscriptionFailedException;
};

/*! @} */
}; // module

#endif
