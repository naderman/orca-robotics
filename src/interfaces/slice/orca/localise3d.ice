/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *                         Anindha Parthy
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LOCALISE_3D_INTERFACE_ICE
#define ORCA2_LOCALISE_3D_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>
#include <orca/stochastic.ice>
#include <orca/position3d.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_localise3d Localise3d
    @brief Multi-hypothesis 3D localisation.
    @{
*/

//! A Gaussian distribution over pose space
struct Pose3dHypothesis {
    //! The mean of the distribution
    Frame3d      mean;
    //! The Gaussian covariance
    Covariance3d cov;
    //! The weight of this component when used as part of a mixture
    float weight;
};

//! A set of Gaussian pose hypotheses: essentially a mixture of Gaussians
sequence<Pose3dHypothesis> Pose3dHypotheses;

//! Localise3d data structure
struct Localise3dData
{
    //! Time when data was measured.
    Time timeStamp;

    //! Localisation hypotheses.
    Pose3dHypotheses hypotheses;
};

/*!
 * Data consumer interface.
 */
interface Localise3dConsumer
{
    //! Transmits the data to the consumer.
    void setData( Localise3dData obj );
};

/*!
 *   @brief Access pose of 3d mobile robotic bases.
 */
interface Localise3d
{
    //! Returns the latest data.
    nonmutating Localise3dData getData()
            throws DataNotExistException;

    //! Returns the data at a particular time
    nonmutating Localise3dData getDataAtTime(Time timeStamp)
            throws DataNotExistException;

    /*!
     * Mimics IceStorm's subscribe(). @p subscriber is typically a direct proxy
     * to the consumer object. The implementation may choose to implement the
     * push directly or use IceStorm. This choice is transparent to the
     * subscriber. The case when the @p subscriber is already subscribed is
     * quietly ignored.
     *
     * @see unsubscribe
     */
    void subscribe( Localise3dConsumer* subscriber )
        throws SubscriptionFailedException;

    /*!
     * Unsubscribe an existing @p subscriber. The case when the @p subscriber
     * is not subscribed is quietly ignored.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( Localise3dConsumer* subscriber );
};


//!  //@}
}; // module 

#endif
