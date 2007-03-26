/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef VELOCITYCONTROL2DSTATEDATA_H
#define VELOCITYCONTROL2DSTATEDATA_H

#include <localnavutil/istatedata.h>
#include <orca/velocitycontrol2d.h>
#include <orca/odometry2d.h>

namespace localnav {

//!
//! @author Alex Brooks, Jonathan Sprinkle
//!

//! Specific implementation of IStateData for @ref VelocityControl2d
class VelocityControl2dStateData : public IStateData
{

public: 
  // FIXME: Alex did not have this in his sensordata version...should he have??
  VelocityControl2dStateData( ) : velocityControl2dData_() { }
  virtual ~VelocityControl2dStateData(){};
  // Copy constructor
  VelocityControl2dStateData( const VelocityControl2dStateData& data )
  : velocityControl2dData_( data.velocityControl2dData_ ) { }

  //! Record the VelocityControl2dDataPtr in a member variable
  void setData( const orca::Odometry2dData& velocityControl2dData ){ velocityControl2dData_ = velocityControl2dData; };
  //! Get the VelocityControl2dDataPtr
  orca::Odometry2dData velocityControl2dData() const { return velocityControl2dData_; };

  //! get the timestamp
  virtual const orca::Time &timeStamp() const { return velocityControl2dData_.timeStamp; }
    
  virtual std::string toString( ) const
  {
    std::stringstream out;
    out << orcaice::toString(velocityControl2dData_);
    return out.str( );
  }
private: 

  orca::Odometry2dData velocityControl2dData_;

};

}

#endif // #ifndef VELOCITYCONTROL2DSTATEDATA_H
