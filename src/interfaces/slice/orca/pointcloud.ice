/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Waleed Kadous, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_POINT_CLOUD_INTERFACE_ICE
#define ORCA2_POINT_CLOUD_INTERFACE_ICE

#include "orca/orca.ice" 
#include "orca/bros1.ice"

module orca {
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_pointcloud PointCloud
    @brief Interface for point cloud producers and consumers

The Pointcloud interface supports devices that generate point clouds of some kind. This 
can include range imagers, data loaded from files, or 3d scanning lasers. 

Because of the wide variety of devices, it is difficult to define a
single configuration interface. Particular devices may wish to
subclass the point cloud interface.

    @{
*/

/*! Currently supports uncoloured and coloured point clouds. 
*/ 

enum PointCloudType { PointCloudTypeNoColour, 
                      PointCloudTypeColour }; 

//! Description of the point cloud
struct PointCloudDescription
{
    //! Time when data was measured.
    Time timeStamp;
  
    //! type of point cloud: colour or uncoloured
    PointCloudType type;
    
    //! Offset of the centre of the sensor from the robot, in the robot coord system
    Frame3d offset; 
    
    //! Dimensions of the sensor
    Size3d size; 

};

//! Point cloud data structure
struct PointCloudData
{
    //! Time when data was measured.
    Time timeStamp;

    //! type of point cloud: colour or uncoloured
    PointCloudType type; 

/*! Points are interleaved. if the Type is PointCloudTypeNoColour, then the sequence
    is X0,Y0,Z0,X1,Y1,Z1... If the type is PointCloudTypeColour, then it is
    X0,Y0,Z0,R0,G0,B0,.... Colours are specified between 0.0 and 1.0  

While it would be nice from a software engineering point of view
to have a collection of points as structs or classes, OpenGL works
better with an interleaved arrays, otherwise we'd be spending a lot of
time just packing and unpacking stuff. 
*/ 
    FloatSeq points;
};

/*! 
    @brief Point cloud consumer 
*/
interface PointCloudConsumer 
{
  void setData( PointCloudData data );   
};

/*!
    @brief Interface for a 3d point cloud
*/
interface PointCloud
{
    //! Returns the latest data.
    //! May raise DataNotExistException if the requested information is not available.
    ["cpp:const"] idempotent PointCloudData     getData()
            throws DataNotExistException; 

    //! Returns device description
    ["cpp:const"] idempotent PointCloudDescription getDescription();  

    /*!
     * Mimics IceStorm's subscribe(). @p subscriber is typically a direct proxy to the consumer object.
     * The implementation may choose to implement the push directly or use IceStorm.
     * This choice is transparent to the subscriber. The case when the @p subscriber is already subscribed
     * is quietly ignored.
     *
     * @see unsubscribe
     */
    void subscribe( PointCloudConsumer *subscriber ) 
            throws SubscriptionFailedException;

    /*!
     * Unsubscribe an existing @p subscriber. The case when the @p subscriber is not subscribed
     * is quietly ignored.
     *
     * @see subscribe
     */
    idempotent void unsubscribe( PointCloudConsumer *subscriber ); 
};

/*! @} */
}; //module

#endif

