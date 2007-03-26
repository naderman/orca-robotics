/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ICONTROLDATA_H
#define ICONTROLDATA_H

#include <orca/orca.h>
#include <iostream>

namespace localnav {
  
// forward declarations
class IStateData;

//!
//! @author Alex Brooks, Jonathan Sprinkle
//!

//! Abstract class for wrapping the controller data for controllers such as @ref DriveBicycle and 
//! @ref VelocityControl2d. Pathplanning drivers return data in this abstract form when returning
//! the 'next' command.
//!
//! All controller data should inherit from this class.
class IControlData
{

public: 

  virtual ~IControlData(){};
  /// Make our own value equal to the stop command
  virtual void setStopCommand( IStateData* currentCommand )=0;
  
  virtual std::string toString( ) const=0;
 
  friend std::ostream& operator<<( std::ostream& out, const IControlData& rhs )
  {
    out << rhs.toString( );
    return out;
  }

private: 


};

}

#endif // #ifndef ICONTROLDATA_H
