/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_POLARFEATURE_INTERFACE_ICE
#define ORCA2_POLARFEATURE_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_polarfeature2d PolarFeature2d
    @brief Range/bearing features relative to robot frame

This interface provides access to features in a polar coordinate system
centered on the robots centre. The data returned by the interface consists of a sequence of
features with range in meters and bearing in rad (-pi<angle<pi) and the feature type.
    @{
*/

//! A single polar feature.
//! If a new feature has more attributes, derive from this.
//!
//! 
//!
class SinglePolarFeature2d
{
    //! Feature location.
    PolarPoint2d p;

    //! Feature type
    //! see @ref orca_interface_featuremap2d for orca-defined feature-types.
    //! This is an int rather than an enum to allow users to make up
    //! non-orca-defined feature types.
    int          type;

    //! The probability of detecting the feature when it's not there
    //! [ p(obs=true|exists=false) ]
    //! This should be a number between:
    //!   - 0 (sensor never screws up), and
    //!   - 1 (sensor always screws up)
    double pFalsePositive;

    //! The probability of detecting the feature when it is there
    //! [ p(obs=true|exists=true) ]
    //! This should be a number between:
    //!   - 0 (sensor will never find features when they're there), and
    //!   - 1 (sensor always finds features when they're there)
    double pTruePositive;
};

//! A sequence of individual features
sequence<SinglePolarFeature2d> PolarFeature2dSequence;

//! A single data transmission may include a sequence of 2D polar features
class PolarFeature2dData extends OrcaObject
{
    //! Features.
    PolarFeature2dSequence features;
};

/*!
 * Data consumer interface (needed only for the push pattern).
 *
 * In Orca-1 terms, this the Consumer side of the ClientPush interface.
 */
interface PolarFeature2dConsumer
{
    //! Transmits the data to the consumer.
    void setData( PolarFeature2dData obj );
};

//! Interface to features in a polar coordinate system.
interface PolarFeature2d
{
    //! Returns the latest data.
    //! @note In Orca1 this would be called ClientPull_Supplier interface.
    nonmutating PolarFeature2dData getData()
            throws DataNotExistException;

    /*!
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the data push internally
     * or use IceStorm. This choice is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * @see unsubscribe
     */
    void subscribe( PolarFeature2dConsumer *subscriber );
    
    /*!
     * Unsubscribe the given @p subscriber.
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( PolarFeature2dConsumer *subscriber );
};

//!  //@}
}; // module

#endif
