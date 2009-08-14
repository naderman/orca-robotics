#ifndef HYDROFEATUREOBS_IFEATUREOBSPUBLISHER_H
#define HYDROFEATUREOBS_IFEATUREOBSPUBLISHER_H

#include <hydrofeatureobs/features.h>
#include <vector>

namespace hydrofeatureobs {

//!
//! @brief Publishes feature observation information (eg. to some viewer application)
//!
//! @author Alex Brooks
//!
class IFeatureObsPublisher
{
public: 

    IFeatureObsPublisher() {}
    virtual ~IFeatureObsPublisher() {}

    virtual void publish( const std::vector<FeatureObs*> &features )=0;
};

//!
//! Does nothing
//!
class NullFeatureObsPublisher : public IFeatureObsPublisher {
public:
    NullFeatureObsPublisher() {}
    void publish( const std::vector<FeatureObs*> &features ) {}
};

}

#endif
