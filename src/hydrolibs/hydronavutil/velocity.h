/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef HYDRONAVUTIL_VELOCITY_H
#define HYDRONAVUTIL_VELOCITY_H

#include <string>
#include <iostream>

namespace hydronavutil {

//!
//! @brief %Velocity of a 2D robot with no side-slip
//!
//! @author Alex Brooks
//!
class Velocity
{

public: 
    //! Empty constructor
    Velocity() {};
    //! Constructor
    Velocity( double lin, double rot )
        : lin_(lin), rot_(rot) {}

    //! Read access
    double lin() const { return lin_; }
    //! Read access
    double rot() const { return rot_; }

    //! Write access
    double &lin() { return lin_; }
    //! Write access
    double &rot() { return rot_; }

private: 

    double lin_, rot_;

};
std::ostream &operator<<( std::ostream &s, const Velocity &v );

//!
//! @brief Acceleration of a 2D robot with no side-slip
//!
//! This thing is identical to velocity...  Tried to typedef but it didn't work.
//!
//! @author Alex Brooks
//!
class Acceleration
{

public: 

    //! Empty constructor
    Acceleration() {};
    //! Constructor
    Acceleration( double lin, double rot )
        : lin_(lin), rot_(rot) {}
    
    //! Read access
    double lin() const { return lin_; }
    //! Read access
    double rot() const { return rot_; }

    //! Write access
    double &lin() { return lin_; }
    //! Write access
    double &rot() { return rot_; }

private: 

    double lin_, rot_;

};
std::ostream &operator<<( std::ostream &s, const Acceleration &v );

}

#endif
