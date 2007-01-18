#ifndef VELOCITY_H
#define VELOCITY_H

#include <string>
#include <iostream>

namespace localnav {

//!
//! @author Alex Brooks
//!
class Velocity
{

public: 

    Velocity() {};
    Velocity( double lin, double rot )
        : lin_(lin), rot_(rot) {}
    
    double lin() const { return lin_; }
    double rot() const { return rot_; }

    double &lin() { return lin_; }
    double &rot() { return rot_; }

private: 

    double lin_, rot_;

};
std::ostream &operator<<( std::ostream &s, const Velocity &v );

//!
//! This thing is identical to velocity...  Tried to typedef but it didn't work.
//!
//! @author Alex Brooks
//!
class Acceleration
{

public: 

    Acceleration() {};
    Acceleration( double lin, double rot )
        : lin_(lin), rot_(rot) {}
    
    double lin() const { return lin_; }
    double rot() const { return rot_; }

    double &lin() { return lin_; }
    double &rot() { return rot_; }

private: 

    double lin_, rot_;

};
std::ostream &operator<<( std::ostream &s, const Acceleration &v );

}

#endif
