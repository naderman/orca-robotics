#ifndef ORCANAVUTIL_POSE_H
#define ORCANAVUTIL_POSE_H

#include <cmath>
#include <iostream>
#include <string>

namespace orcanavutil {

//!
//! @author 
//!
class Pose
{

public: 

    Pose() {};
    Pose( double x, double y, double theta )
        :x_(x),y_(y),theta_(theta) {}

    // non-const versions
    double &x()     { return x_; }
    double &y()     { return y_; }
    double &theta() { return theta_; }
    
    // const versions
    double x()     const { return x_; }
    double y()     const { return y_; }
    double theta() const { return theta_; }

    std::string toString() const;

private: 

    double x_, y_, theta_;

};

std::ostream &operator<<( std::ostream &s, const Pose &p );

}

#endif
