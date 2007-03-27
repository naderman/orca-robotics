/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef DRIVEBICYCLESTATEDATA_H
#define DRIVEBICYCLESTATEDATA_H

#include <localnavutil/istatedata.h>
#include <orca/drivebicycle.h>
#include <orcaice/orcaice.h>

namespace localnav {

//!
//! @author Alex Brooks, Jonathan Sprinkle
//!

//! Specific implementation of IStateData for @ref DriveBicycle
class DriveBicycleStateData : public IStateData
{

public: 
  // FIXME: Alex did not have this in his sensordata version...should he have??
   DriveBicycleStateData( ) : bicycleData_() { }
  // FIXME: not declared virtual by Alex's code
  virtual ~DriveBicycleStateData(){};
  DriveBicycleStateData( const DriveBicycleStateData& that )
    : bicycleData_(that.bicycleData_) { }
  
    virtual const orca::Time &timeStamp() const { return bicycleData_.timeStamp; }
    
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

#endif // #ifndef DRIVEBICYCLESTATEDATA_H
