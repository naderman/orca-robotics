/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

    An occupancy grid fusion interface which receives occupancy likelihoods per grid cell
    for fusing them into an occupancy grid map.

*/
struct OgFusionConfig
{
    //! Time when data was measured.
    Time timeStamp;

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

//! A basic structure for individual likelihoods per cell
struct OgCellLikelihood
{
   int x;
   int y;
   float likelihood;
};

//! A sequence of likelihoods forms an observation
sequence<OgCellLikelihood>         OgObservation;

/*!
    OgFusionData simply contains a sequence of occupancy likelihoods per cell
*/
struct OgFusionData
{
    //! Time when data was measured.
    Time timeStamp;

    //! A sequence of cell likelihoods is an observation
    OgObservation observation;
};

/*!
    The OgFusion interface receives sequences of cell
    likelihoods in form of OgFusionData. It can also be
    queried for its configuration.
 */
interface OgFusion
{
    //! retrieves map configuration etc.
    nonmutating OgFusionConfig getConfig();
    
    //! set an observation
    void setData(OgFusionData data);
};

/*! @} */
}; // module

#endif
