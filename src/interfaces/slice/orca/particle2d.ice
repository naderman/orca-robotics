/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_PARTICLE2D_INTERFACE_ICE
#define ORCA2_PARTICLE2D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>
#include <orca/position2d.ice>

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
    Laser scan data structure
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
    @brief Interface for a 2d particle-based localiser
 */
interface Particle2d extends Position2d
{
    void subscribeForParticles( Particle2dConsumer *subscriber );
    idempotent void unsubscribeForParticles( Particle2dConsumer *subscriber );

//     void subscribe( Particle2dConsumer *subscriber );
//     idempotent void unsubscribe( Particle2dConsumer *subscriber );
};

/*! @} */
}; // module

#endif
