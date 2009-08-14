#ifndef HYDROSIM2D_IPOSEPUBLISHER_H
#define HYDROSIM2D_IPOSEPUBLISHER_H

#include <hydronavutil/hydronavutil.h>

namespace hydrosim2d {

//!
//! @brief Publishes pose information (eg. to some viewer application)
//!
//! @author Alex Brooks
//!
class IPosePublisher
{
public: 

    virtual ~IPosePublisher() {}

    // Default implementation does nothing.
    virtual void publish( const hydronavutil::Pose &pose ) {}
};

}

#endif
