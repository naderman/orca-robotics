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

#ifndef ORCA2_LOCALISE_2D_INTERFACE_ICE
#define ORCA2_LOCALISE_2D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>
#include <orca/stochastic.ice>
#include <orca/position2d.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_localise2d Localise2d
    @brief Multi-hypothesis planar localisation.
    @{
*/

//! A Gaussian distribution over pose space
struct Pose2dHypothesis {
    //! The mean of the distribution
    Frame2d      mean;
    //! The Gaussian covariance
    Covariance2d cov;
    //! The weight of this component when used as part of a mixture
    float weight;
};

//! A set of Gaussian pose hypotheses: essentially a mixture of Gaussians
sequence<Pose2dHypothesis> Pose2dHypotheses;

//! Localise2d data structure
class Localise2dData extends OrcaObject
{
    //! Localisation hypotheses.
    Pose2dHypotheses hypotheses;
};

/*!
 * Data consumer interface.
 */
interface Localise2dConsumer
{
    //! Transmits the data to the consumer.
    void setData( Localise2dData obj );
};

/*!
    @brief Access to odometry of 2d mobile robitic bases.
*/
interface Localise2d
{
    //! Returns the latest data.
    nonmutating Localise2dData getData()
            throws DataNotExistException;

    /*!
     * Mimics IceStorm's subscribe(). @p subscriber is typically a direct proxy to the consumer object.
     * The implementation may choose to implement the push directly or use IceStorm.
     * This choice is transparent to the subscriber. The case when the @p subscriber is already subscribed
     * is quietly ignored.
     *
     * @see unsubscribe
     */
    void subscribe( Localise2dConsumer* subscriber )
        throws SubscriptionFailedException;

    // for reference, this is what IceStorm's subscribe function looks like.
    //void subscribe(QoS theQoS, Object* subscriber);

    /*!
     * Unsubscribe an existing @p subscriber. The case when the @p subscriber is not subscribed
     * is quietly ignored.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( Localise2dConsumer* subscriber );
};


//!  //@}
}; // module

#endif
