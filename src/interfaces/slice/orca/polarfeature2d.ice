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
