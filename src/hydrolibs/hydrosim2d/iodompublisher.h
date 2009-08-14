#ifndef HYDROSIM2D_IODOMPUBLISHER_H
#define HYDROSIM2D_IODOMPUBLISHER_H

#include <hydronavutil/hydronavutil.h>

namespace hydrosim2d {

//!
//! @brief Publishes odom information (eg. to some viewer application)
//!
//! @author Alex Brooks
//!
class IOdomPublisher
{
public: 

    virtual ~IOdomPublisher() {}

    // Default implementation does nothing.
    virtual void publish( const hydronavutil::Pose     &pose,
                          const hydronavutil::Velocity &velocity ) {}
};

}

#endif
