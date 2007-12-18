#ifndef ORCA_POINT_CLOUD_CONSUMER_I_H
#define ORCA_POINT_CLOUD_CONSUMER_I_H

#include <orca/pointcloud.h>

namespace pcviewer
{

class PointCloudConsumerI : virtual public orca::PointCloudConsumer
{
public:

  virtual void setData(const ::orca::PointCloudData&, const Ice::Current&);
  PointCloudConsumerI(std::string endpoint);

  orca::PointCloudData getPointCloud(); 
  void init(int argc, char **argv);
private: 
  std::string endpoint;
  orca::PointCloudPrx pcpPrx;
  orca::PointCloudConsumerPrx myPrx;
  Ice::ObjectAdapterPtr adapter;
};
}

#endif
