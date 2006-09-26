#ifndef ORCA2_POINT_CLOUD_CONSUMER_I_H
#define ORCA2_POINT_CLOUD_CONSUMER_I_H

#include <orca/pointcloud.h>

namespace orca
{

class PointCloudConsumerI : virtual public PointCloudConsumer
{
public:

  virtual void setData(const ::orca::PointCloudPtr&,
			 const Ice::Current&);
  PointCloudConsumerI(std::string endpoint);

  PointCloudPtr getPointCloud(); 
  void init(int argc, char **argv);
private: 
  std::string endpoint;
  orca::PointCloudProducerPrx pcpPrx;
  orca::PointCloudConsumerPrx myPrx;
  Ice::ObjectAdapterPtr adapter;
};
}

#endif
