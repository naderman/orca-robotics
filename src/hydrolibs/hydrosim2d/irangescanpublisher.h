#ifndef HYDROSIM2D_IRANGESCANPUBLISHER_H
#define HYDROSIM2D_IRANGESCANPUBLISHER_H

#include <vector>

namespace hydrosim2d {

//!
//! @brief Publishes range scan information (eg. to some viewer application)
//!
//! @author Alex Brooks
//!
class IRangeScanPublisher
{
public: 

    virtual ~IRangeScanPublisher() {}

    virtual void publish( const std::vector<float> &ranges )=0;
};

//!
//! Does nothing
//!
class NullRangeScanPublisher : public IRangeScanPublisher {
public:
    NullRangeScanPublisher() {}
    void publish( const std::vector<float> &ranges ) {}
};

}

#endif
