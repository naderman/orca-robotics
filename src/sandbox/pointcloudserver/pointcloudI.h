#ifndef __pointcloudI_h__
#define __pointcloudI_h__

#include <pointcloud.h>

namespace orca
{

class PointCloudConsumerI : virtual public PointCloudConsumer
{
public:

    virtual void setData(const ::orca::PointCloudPtr&,
			 const Ice::Current&);
};

class PointCloudProducerI : virtual public PointCloudProducer
{
public:

    virtual ::orca::PointCloudProducerGeometryPtr getGeometry(const Ice::Current&);

    virtual ::orca::PointCloudPtr getPointCloud(const Ice::Current&);

    virtual void subscribe(const ::orca::PointCloudConsumerPrx&,
			   const Ice::Current&);

    virtual void unsubscribe(const ::orca::PointCloudConsumerPrx&,
			     const Ice::Current&);
};

}

#endif
