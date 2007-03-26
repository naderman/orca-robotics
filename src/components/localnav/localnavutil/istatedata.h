/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ISTATEDATA_H
#define ISTATEDATA_H

#include <orca/orca.h>
#include <iostream>

namespace localnav {

//!
//! @author Alex Brooks, Jonathan Sprinkle
//!

//! Abstract class for wrapping the state data for controllers such as @ref DriveBicycle and 
//! @ref VelocityControl2d. Pathplanning drivers receive data in this abstract form as they
//! make decisions on future control states based on this current state.
//!
//! All state data classes should inherit from this class. Children classes can (obviously)
//! contain more than one kind of structure to make up the entire state space.
class IStateData
{

public: 

  virtual ~IStateData(){};
  
  //! get the timestamp
  virtual const orca::Time &timeStamp() const=0;

  virtual std::string toString( ) const=0;
 
  friend std::ostream& operator<<( std::ostream& out, const IStateData& rhs )
  {
    out << rhs.toString( );
    return out;
  }
  
  
  

private: 


};

}

#endif // #ifndef ICONTROLDATA_H
