/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_OGFUSION_INTERFACE_ICE
#define ORCA2_OGFUSION_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_ogfusion OgFusion
    @brief An occupancy grid fusion interface

    An occupancy grid fusion interface

*/
class OgFusionConfig extends OrcaObject
{
    //! The global coordinates of the bottom-left corner of the bottom-left cell
    Frame2d      origin;

    //! The number of cells along the x axis
    int numCellsX;
    //! The number of cells along the y axis
    int numCellsY;
    //! The size of each cell, in the x dimension
    float metresPerCellX;
    //! The size of each cell, in the y dimension
    float metresPerCellY;
};

//! A basic structure for individual likelihoods
struct OgCellLikelihood
{
   int x;
   int y;
   float likelihood;
};

//! A sequence of likelihoods forms an observation
sequence<OgCellLikelihood>         OgObservation;

/*!
    @brief Blah.
*/
class OgFusionData extends OrcaObject
{
    //! The actual data
    OgObservation observation;
};

/*!
    @brief Blah.

 */
interface OgFusion
{
    //! retreives map configuration etc.
    nonmutating OgFusionConfig getConfig();
    
    //! set an observation
    void setData(OgFusionData data);
};

/*! @} */
}; // module

#endif
