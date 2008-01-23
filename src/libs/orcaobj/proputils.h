/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_PROPERTY_UTILITIES_H
#define ORCAOBJ_PROPERTY_UTILITIES_H

#include <string>
#include <Ice/Ice.h>
#include <orca/orca.h>
#include <orca/bros1.h>

namespace orcaobj
{

/*!
    *  @name Property functions
    */
//@{

//! Parses the value into a sequence of strings using the given delimiter
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsStringSeq( const Ice::PropertiesPtr &, const ::std::string& key, Ice::StringSeq &value, char delim=':' );
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
//! Parses the value into a Time object assuming ':' delimeters.
//! Returns: 0 = property found (and set in value), non-zero = property not found or parsing failed.
int getPropertyAsTimeDuration( const Ice::PropertiesPtr &, const ::std::string& key, orca::Time &value );

//! Returns the default value if key is not found or cannot be converted to a string sequence.
Ice::StringSeq getPropertyAsStringSeqWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const Ice::StringSeq &defaultValue, char delim=':' );
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
//! Returns the default value if key is not found or cannot be converted to a Time.
orca::Time getPropertyAsTimeDurationWithDefault( const Ice::PropertiesPtr &, const ::std::string& key, const orca::Time &d );

//@}

} // namespace

#endif
