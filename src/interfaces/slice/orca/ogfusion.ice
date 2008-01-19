/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
    @ingroup orca_interfaces
    @defgroup orca_interface_ogfusion OgFusion
    @brief An occupancy grid fusion interface
    @{

    An occupancy grid fusion interface which receives occupancy likelihoods per grid cell
    for fusing them into an occupancy grid map.
*/

//! Description of the OgFusion interface
struct OgFusionConfig
{
    //! Time when data was measured.
    Time timeStamp;

    //! Transformation from the global (arbitrarily defined) coordinate system 
    //! (CS) to the CS of the OG map. The OG map CS is 
    //! defined as as follows:
    //! - origin at the bottom-left corner
    //! - X axis along the bottom edge to the right
    //! - Y axis along the left edge up
    //! Example: if the global CS starts in the middle of a OG map and there are
    //! no rotations, then the OG map offset is (-width/2, -height/2, 0).
    Frame2d offset;

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
    //! The x index of the cell
    int x;
    //! The y index of the cell
    int y;
    // The likelihood of occupancy
    float likelihood;
};

//! A sequence of likelihoods forms an observation
sequence<OgCellLikelihood> OgObservation;

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
    ["cpp:const"] idempotent OgFusionConfig getConfig();
    
    //! set an observation
    void setData(OgFusionData data);
};

/*! 
    @} 
*/
}; // module

#endif
