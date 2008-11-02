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

#include <orca/common.ice>
#include <orca/bros1.ice>

module orca {
/*!
    @ingroup orca_interfaces
    @defgroup orca_interface_pointcloud PointCloud
    @brief Interface for point cloud producers and consumers

The PointCloud interface gives access to clouds of 3d points.

    @{
*/

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
  void setData( PointCloudData data );   
};

/*!
    @brief Interface for a 3d point cloud source
*/
interface PointCloud
{
    //! Returns the latest data.
    //! May raise DataNotExistException if the requested information is not available.
    idempotent PointCloudData getData()
            throws DataNotExistException; 

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

