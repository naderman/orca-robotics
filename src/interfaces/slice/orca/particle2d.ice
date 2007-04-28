/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PARTICLE2D_INTERFACE_ICE
#define ORCA2_PARTICLE2D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
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
    ["cpp:const"] idempotent Particle2dData getData()
            throws DataNotExistException;

    /*!
     * Mimics IceStorm's subscribe(). @p subscriber is typically a direct proxy to the consumer object.
     * The implementation may choose to implement the push directly or use IceStorm.
     * This choice is transparent to the subscriber. The case when the @p subscriber is already subscribed
     * is quietly ignored.
     *
     * @see unsubscribe
     */
    void subscribe( Particle2dConsumer* subscriber )
        throws SubscriptionFailedException;

    /*!
     * Unsubscribe an existing @p subscriber. The case when the @p subscriber is not subscribed
     * is quietly ignored.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( Particle2dConsumer* subscriber );
};

/*! @} */
}; // module

#endif
