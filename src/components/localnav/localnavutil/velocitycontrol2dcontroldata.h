/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef VELOCITYCONTROL2DCONTROLDATA_H
#define VELOCITYCONTROL2DCONTROLDATA_H

#include <localnavutil/icontroldata.h>
#include <localnavutil/istatedata.h>
#include <orca/velocitycontrol2d.h>
#include <localnavutil/velocitycontrol2dstatedata.h>

namespace localnav {

//!
//! @author Alex Brooks, Jonathan Sprinkle
//!

//! Specific implementation of IControlData for @ref VelocityControl2d
class VelocityControl2dControlData : public IControlData
{

public: 
  // FIXME: Alex did not have this in his sensordata version...should he have??
  VelocityControl2dControlData( ) : velocityControl2dData_() { }
  ~VelocityControl2dControlData(){};
  // Copy constructor
  VelocityControl2dControlData( const VelocityControl2dControlData& data )
  : velocityControl2dData_( data.velocityControl2dData_ ) { }

  //! Record the VelocityControl2dDataPtr in a member variable
  void setData( const orca::VelocityControl2dData& velocityControl2dData ){ velocityControl2dData_ = velocityControl2dData; };
  //! Get the VelocityControl2dDataPtr
  orca::VelocityControl2dData velocityControl2dData() const { return velocityControl2dData_; };

  /// sets our velocity to 0 in all directions
  /// Note that this is independent of the current command, but we still check to make sure
  /// that the correct kind of command is passed in.
  virtual void setStopCommand( IStateData* currentCmd ) 
  { 
    // do dynamic cast to ensure it is what we are expecting
    const VelocityControl2dStateData *data = dynamic_cast<VelocityControl2dStateData*>( const_cast<IStateData*>(currentCmd) );
    if( data != NULL )
    {
      this->velocityControl2dData_.motion.v.x = 0.0;
      this->velocityControl2dData_.motion.v.y = 0.0;
      this->velocityControl2dData_.motion.w   = 0.0;
    }
    else
    {
      // It's no big deal for this platform, we can do the exact same thing
      // TODO Jon--maybe we should do something to alert the user of this? Or Not?
      this->velocityControl2dData_.motion.v.x = 0.0;
      this->velocityControl2dData_.motion.v.y = 0.0;
      this->velocityControl2dData_.motion.w   = 0.0;
    }
  }

  virtual std::string toString( ) const
  {
    std::stringstream out;
    out << orcaice::toString(velocityControl2dData_);
    return out.str( );
  }
private: 

  orca::VelocityControl2dData velocityControl2dData_;

};

}

#endif // #ifndef VELOCITYCONTROL2DCONTROLDATA_H
