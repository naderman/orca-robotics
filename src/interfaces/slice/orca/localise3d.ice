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

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <orca/stochastic.ice>
#include <orca/vehicledescription.ice>
#include <IceStorm/IceStorm.ice>

module orca
{
/*!
    @ingroup orca_interfaces
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
    //! Get the geometry of the localised vehicle
    idempotent VehicleGeometryDescription getDescription();

    //! Returns the latest data.
    idempotent Localise3dData getData()
            throws DataNotExistException;
    
    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( Localise3dConsumer* subscriber )
        throws SubscriptionFailedException;
};


//!  //@}
}; // module 

#endif
