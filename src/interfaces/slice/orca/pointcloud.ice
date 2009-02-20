/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Waleed Kadous, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_POINTCLOUD_INTERFACE_ICE
#define ORCA2_POINTCLOUD_INTERFACE_ICE

#include <orca/common.ice>
#include <orca/bros1.ice>
#include <IceStorm/IceStorm.ice>

module orca 
{
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_pointcloud PointCloud
    @brief Interface for point cloud producers and consumers

The PointCloud interface gives access to clouds of 3d points.

    @{
*/

//! Device description
struct PointCloudDescription
{
    //! Time when description was generated.
    Time timeStamp;

    //! Offset of the centre of the sensor with respect to the robot, 
    //! in the robot local coordinate system.
    Frame3d offset;
};

//! A sequence of points
sequence<CartesianPoint> CartesianPointSequence;

//! Point cloud data structure
struct PointCloudData
{
    //! Time when data was measured.
    Time timeStamp;

    //! The point cloud: a set of points, all in platform-centred coordinate system.
    CartesianPointSequence points;
};

/*! 
    @brief Point cloud consumer 
*/
interface PointCloudConsumer 
{
    //! Transmits the data to the consumer.
  void setData( PointCloudData obj );   
};

/*!
    @brief Interface for a 3d point cloud source
*/
interface PointCloud
{

    //! Returns device description.
    idempotent PointCloudDescription getDescription();

    //! Returns the latest data.
    //! May raise DataNotExistException if the requested information is not available.
    idempotent PointCloudData getData()
            throws DataNotExistException; 

    //! Tries to subscribe the specified subscriber for data updates.
    //! If successfuly, returns a proxy to the IceStorm topic which can be later used by the 
    //! client to unsubscribe itself. For reference, the Slice definition of the Topic
    //! interface for unsubscribing:
    //! @verbatim
    //! idempotent void unsubscribe(Object* subscriber);
    //! @endverbatim
    IceStorm::Topic* subscribe( PointCloudConsumer* subscriber )
        throws SubscriptionFailedException;
};

/*! @} */
}; //module

#endif

