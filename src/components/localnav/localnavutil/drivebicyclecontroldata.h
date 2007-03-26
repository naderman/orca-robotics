/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef DRIVEBICYCLECONTROLDATA_H
#define DRIVEBICYCLECONTROLDATA_H

#include <localnavutil/istatedata.h>
#include <localnavutil/icontroldata.h>
#include <orca/drivebicycle.h>

namespace localnav {

//!
//! @author Alex Brooks, Jonathan Sprinkle
//!

//! Specific implementation of IControlData for @ref DriveBicycle
class DriveBicycleControlData : public IControlData
{

public: 
  // FIXME: Alex did not have this in his sensordata version...should he have??
  DriveBicycleControlData( ) : bicycleData_() { }
  // FIXME: not declared virtual by Alex's code
  virtual ~DriveBicycleControlData(){};
  DriveBicycleControlData( const DriveBicycleControlData& that )
  : bicycleData_(that.bicycleData_) { }
  
  /// sets our velocity to 0, while maintaining current steering angle
  /// FIXME: determine if this is what we actually want!?
  virtual void setStopCommand( IStateData* currentCmd );
  
  virtual std::string toString( ) const
  {
    std::stringstream out;
    out << orcaice::toString(bicycleData_);
    return out.str( );
  }


  //! Record the DriveBicycleData in a member variable
  void setData( const orca::DriveBicycleData& bicycleData ){ bicycleData_ = bicycleData; };
  //! Get the DriveBicycleData
  orca::DriveBicycleData driveBicycleData() const { return bicycleData_; };

private: 

  orca::DriveBicycleData bicycleData_;

};

}

#endif // #ifndef DRIVEBICYCLECONTROLDATA_H
