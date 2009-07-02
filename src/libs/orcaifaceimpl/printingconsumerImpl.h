/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAIFACEIMPL_PRINTING_CONSUMER_IMPL_H
#define ORCAIFACEIMPL_PRINTING_CONSUMER_IMPL_H

#include <orcaifaceimpl/consumerImpl.h>

// all of these are here to define toString()
// alexm: I haven't checked which ones are actually used.
#include <orcaifaceutil/datetime.h>
#include <orcaifaceutil/exceptions.h>
#include <orcaifaceutil/containers.h>
#include <orcaifaceutil/common.h>
#include <orcaifaceutil/ocm.h>
#include <orcaifaceutil/bros1.h>
#include <orcaifaceutil/stochastic.h>
#include <orcaifaceutil/button.h>
#include <orcaifaceutil/camera.h>
#include <orcaifaceutil/cpu.h>
#include <orcaifaceutil/drivebicycle.h>
#include <orcaifaceutil/drivedifferential.h>
#include <orcaifaceutil/estop.h>
#include <orcaifaceutil/featuremap2d.h>
#include <orcaifaceutil/gps.h>
#include <orcaifaceutil/home.h>
#include <orcaifaceutil/image.h>
#include <orcaifaceutil/ins.h>
#include <orcaifaceutil/imu.h>
#include <orcaifaceutil/localise2d.h>
#include <orcaifaceutil/localise3d.h>
#include <orcaifaceutil/multicamera.h>
#include <orcaifaceutil/rangerarray.h>
#include <orcaifaceutil/odometry2d.h>
#include <orcaifaceutil/odometry3d.h>
#include <orcaifaceutil/ogfusion.h>
#include <orcaifaceutil/ogmap.h>
#include <orcaifaceutil/particle2d.h>
#include <orcaifaceutil/pathfollower2d.h>
#include <orcaifaceutil/pathplanner2d.h>
#include <orcaifaceutil/pinger.h>
#include <orcaifaceutil/pixmap.h>
#include <orcaifaceutil/pointcloud.h>
#include <orcaifaceutil/polarfeature2d.h>
#include <orcaifaceutil/power.h>
#include <orcaifaceutil/properties.h>
#include <orcaifaceutil/qgraphics2d.h>
#include <orcaifaceutil/rangescanner2d.h>
#include <orcaifaceutil/laserscanner2d.h>
#include <orcaifaceutil/status.h>
#include <orcaifaceutil/systemstatus.h>
#include <orcaifaceutil/tracer.h>
#include <orcaifaceutil/vehicledescription.h>
#include <orcaifaceutil/velocitycontrol2d.h>
#include <orcaifaceutil/wifi.h>

namespace orcaifaceimpl
{

/*!
 *  @brief A convenience class for consumers with setData() operation.
 *
 *  When the data arrives it does one thing: puts out data as text through
 *  as an info message of Tracer.
 *
 *  Relies on the fact that the Consumer objects has only one operation
 *  to implement and it's called setData() and toString() function is implemented
 *  for the data type.
 */
// template<class ProviderType, class ProviderPrxType, class ConsumerType, class ConsumerPrxType, class ObjectType>
template<class ProviderType, class ConsumerType, class ObjectType>
class PrintingConsumerImpl : 
        public ConsumerImpl<ProviderType,ConsumerType,ObjectType>
//         public ConsumerImpl<ProviderType,ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>
{
// using ConsumerImpl<ProviderType,ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>::context_;
using ConsumerImpl<ProviderType,ConsumerType,ObjectType>::context_;
public:
    //! Constructor. The optinal parameter @c recurse specifies the level of detail printed
    //! out to @c cout when new data arrives.
    PrintingConsumerImpl( const orcaice::Context &context, int recurse=1000, int expand=-1 ) :
        ConsumerImpl<ProviderType,ConsumerType,ObjectType>(context), 
//         ConsumerImpl<ProviderType,ProviderPrxType,ConsumerType,ConsumerPrxType,ObjectType>(context), 
        recurse_(recurse),
        expand_(expand) {}

    //! This callback simply prints it out to standard output.
    virtual void dataEvent( const ObjectType& data ) 
    {
        std::cout << "PrintingConsumerImpl::dataEvent() start" << std::endl;
        context_.tracer().info( ifaceutil::toString(data,recurse_,expand_) ); 
    }
private:
    int recurse_;
    int expand_;
};

} // namespace

#endif
