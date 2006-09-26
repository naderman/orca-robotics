
#ifndef ORCA2_POINTCLOUDSERVER_H
#define ORCA2_POINTCLOUDSERVER_H
#include <orca/pointcloud.h>


namespace orca
{


class PointCloudServer : virtual public PointCloudProducer
{
public:

    PointCloudServer(char *fileName);
    virtual ::orca::PointCloudProducerGeometryPtr getGeometry(const Ice::Current&);

    virtual ::orca::PointCloudPtr getPointCloud(const Ice::Current&);

    virtual void subscribe(const ::orca::PointCloudConsumerPrx&,
			   const Ice::Current&);

    virtual void unsubscribe(const ::orca::PointCloudConsumerPrx&,
			     const Ice::Current&);

private: 
    std::vector<float> currentPointCloud; 
    char *fileName; 
    int readFromVRML(const char * filename, std::vector <float> * points);
    void generateSphere(std::vector<float> *points);
};

};

#endif
