
#include <pointcloudserver.h>

::orca::PointCloudProducerGeometryPtr
orca::PointCloudProducerI::getGeometry(const Ice::Current& current)
{
    return 0;
}

::orca::PointCloudPtr
orca::PointCloudProducerI::getPointCloud(const Ice::Current& current)
{
    return 0;
}

void
orca::PointCloudProducerI::subscribe(const ::orca::PointCloudConsumerPrx& consumer,
				     const Ice::Current& current)
{

}

void
orca::PointCloudProducerI::unsubscribe(const ::orca::PointCloudConsumerPrx& consumer,
				       const Ice::Current& current)
{
}
