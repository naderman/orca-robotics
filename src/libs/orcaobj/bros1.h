/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_BROS1_H
#define ORCAOBJ_BROS1_H

#include <orca/bros1.h>
#include <Ice/Properties.h>

namespace orcaobj
{

//! Sets all fields to specified values
void setInit( orca::Frame2d&, double x=0.0, double y=0.0, double orient=0.0 );
//! Sets all fields to specified values
void setInit( orca::Frame3d&, double x=0.0, double y=0.0, double z=0.0, double pitch=0.0, double roll=0.0, double yaw=0.0 );
//! Sets all fields to specified values
void setInit( orca::Twist2d&, double vx=0.0, double vy=0.0, double w=0.0 );
//! Sets all fields to specified values
void setInit( orca::Twist3d&, double vx=0.0, double vy=0.0, double vz=0.0, double wx=0.0, double wy=0.0, double wz=0.0 );
//! Sets all fields to specified values
void setInit( orca::Size2d&, double l=0.0, double w=0.0 );
//! Sets all fields to specified values
void setInit( orca::Size3d&, double l=0.0, double w=0.0, double h=0.0 );
//! Sets all fields to specified values
void setInit( orca::CartesianPoint2d&, double x=0.0, double y=0.0 );
//! Sets all fields to specified values
void setInit( orca::CartesianPoint&, double x=0.0, double y=0.0, double z=0.0 );

//! Converts to string
//! @see toCartesianPoint()
std::string toString( const orca::CartesianPoint& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toCartesianPoint( const std::string &, orca::CartesianPoint& );

//! Converts to string
//! @see toCartesianPoint2d()
std::string toString( const orca::CartesianPoint2d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toCartesianPoint2d( const std::string &, orca::CartesianPoint2d& );

//! Converts to string
//! @see toFrame2d()
std::string toString( const orca::Frame2d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toFrame2d( const std::string &, orca::Frame2d& );

//! Converts to string
//! @see toFrame3d()
std::string toString( const orca::Frame3d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toFrame3d( const std::string &, orca::Frame3d& );

//! Converts to string
//! @see toSize2d()
std::string toString( const orca::Size2d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toSize2d( const std::string &, orca::Size2d& );

//! Converts to string
//! @see toSize3d()
std::string toString( const orca::Size3d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toSize3d( const std::string &, orca::Size3d& );

//! Converts to string
std::string toString( const orca::Twist2d& );

//! Parses the value into a Frame2d object assuming empty space delimeters. The format is assumed to
//! be: x[m] y[m] yaw[deg]. 
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsFrame2d( const Ice::PropertiesPtr &, const ::std::string& key, orca::Frame2d &value );
//! Parses the value into a Frame3d object assuming empty space delimeters. The format is assumed to
//! be: x[m] y[m] z[m] roll[deg] pitch[deg] yaw[deg]. 
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsFrame3d( const Ice::PropertiesPtr &, const ::std::string& key, orca::Frame3d &value );
//! Parses the value into a Size2d object assuming empty space delimeters.
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsSize2d( const Ice::PropertiesPtr &, const ::std::string& key, orca::Size2d &value );
//! Parses the value into a Size3d object assuming empty space delimeters.
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsSize3d( const Ice::PropertiesPtr &, const ::std::string& key, orca::Size3d &value );
//! Parses the value into a CartesianPoint object assuming empty space delimeters.
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsCartesianPoint( const Ice::PropertiesPtr &, const ::std::string& key, orca::CartesianPoint &value );

//! Returns the default value if key is not found or cannot be converted to a Frame2d.
orca::Frame2d getPropertyAsFrame2dWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const orca::Frame2d &defaultValue );
//! Returns the default value if key is not found or cannot be converted to a Frame3d.
orca::Frame3d getPropertyAsFrame3dWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const orca::Frame3d &defaultValue );
//! Returns the default value if key is not found or cannot be converted to a Size3d.
orca::Size2d getPropertyAsSize2dWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const orca::Size2d &defaultValue );
//! Returns the default value if key is not found or cannot be converted to a Size3d.
orca::Size3d getPropertyAsSize3dWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const orca::Size3d &defaultValue );
//! Returns the default value if key is not found or cannot be converted to a CartesianPoint.
orca::CartesianPoint getPropertyAsCartesianPointWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const orca::CartesianPoint &defaultValue );

//! Rotate a 2d point by an angle (about Z-axis)
orca::CartesianPoint2d rotate2d(const orca::CartesianPoint2d &point, const double &angle);

//! Rotate a 3d point by an angle about the X axis
orca::CartesianPoint rotate3dX(const orca::CartesianPoint &point, const double &angle);

//! Rotate a 3d point by an angle about the Y axis
orca::CartesianPoint rotate3dY(const orca::CartesianPoint &point, const double &angle);

//! Rotate a 3d point by an angle about the Z axis
orca::CartesianPoint rotate3dZ(const orca::CartesianPoint &point, const double &angle);

//! Convert a global point to another, relative to a frame
orca::CartesianPoint2d convertToFrame2d(const orca::Frame2d &frame, const orca::CartesianPoint2d &point);

//! Convert a global point to another, relative to a frame
orca::CartesianPoint convertToFrame3d(const orca::Frame3d &frame, const orca::CartesianPoint &point);

//! Convert a point relative to a frame back to the global frame
orca::CartesianPoint2d convertFromFrame2d(const orca::Frame2d &frame, const orca::CartesianPoint2d &point);

//! Convert a point relative to a frame back to the global frame
orca::CartesianPoint convertFromFrame3d(const orca::Frame3d &frame, const orca::CartesianPoint &point);

//! Returns a frame 3d filled out with all zeros.
// TODO: replace with autogenerated zeroAndClear()
orca::Frame3d zeroFrame3d();

} // namespace

#endif
