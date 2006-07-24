/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PARTICLE2D_INTERFACE_ICE
#define ORCA2_PARTICLE2D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>
#include <orca/localise2d.ice>

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
    Frame2d pose;
    float   weight;
};

//! A sequence of particles
sequence<SingleParticle2d>  Particle2dSequence;

/*!
    Particle2d data structure
*/
class Particle2dData extends OrcaObject
{
    Particle2dSequence particles;
};

/*!
    @brief Consumer of 2d particles
*/
interface Particle2dConsumer
{
    void setData( Particle2dData obj );
};

/*!
    @brief Interface for a 2d particle-based localiser.
           Extends the mixture-of-Gaussians interface Localise2d,
           Adding the particle-based stuff.
 */
interface Particle2d extends Localise2d
{
    //! 
    void subscribeForParticles( Particle2dConsumer *subscriber );

    //! 
    idempotent void unsubscribeForParticles( Particle2dConsumer *subscriber );
};

/*! @} */
}; // module

#endif
