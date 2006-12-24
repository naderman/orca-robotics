
#ifndef ORCA2_POINT_CLOUD_I_H
#define ORCA2_POINT_CLOUD_I_H

#include <orca/pointcloud.h>

namespace pcserver
{

class PointCloudI : virtual public orca::PointCloud
{
public:

    PointCloudI(char *fileName);
    virtual ::orca::PointCloudDescription getDescription(const Ice::Current&) const;

    virtual ::orca::PointCloudData getData(const Ice::Current&) const;

    virtual void subscribe(const ::orca::PointCloudConsumerPrx&,
			   const Ice::Current&);

    virtual void unsubscribe(const ::orca::PointCloudConsumerPrx&,
			     const Ice::Current&);

private: 
    char *filename_; 

    // hack with 'mutabale' and 'const' to let us call these functions from
    // within getData() which is const. We can also remove the const qualifier
    // from the interface definition.
    mutable std::vector<float> currentPointCloud_; 

    int readFromVRML(const char * filename, std::vector <float> * points) const;

    void generateSphere( std::vector<float> *points ) const;
};

};

#endif
