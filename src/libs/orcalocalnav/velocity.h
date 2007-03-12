/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_VELOCITY_H
#define ORCA_VELOCITY_H

#include <string>
#include <iostream>

namespace orcalocalnav {

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
