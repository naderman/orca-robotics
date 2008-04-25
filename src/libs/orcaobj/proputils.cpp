/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include <stdlib.h>             // for getenv()

#include <orcaice/proputils.h>
#include <orcaobj/stringutils.h>        
#include <hydroutil/stringutils.h>    // for toStringSeq()
#include <gbxsickacfr/gbxutilacfr/mathdefs.h>           // for DEG2RAD_RATIO

#include "proputils.h"
// #include "configutils.h"
// #include "exceptions.h"
// #include "printutils.h"       // initTracerInfo()

using namespace std;

namespace orcaobj
{

int
getPropertyAsStringSeq( const Ice::PropertiesPtr & prop, const ::std::string& key, Ice::StringSeq & value, char delim )
{
    std::string stringVal;
    if ( orcaice::getProperty( prop, key, stringVal ) ) {
        return -1;
    }
    else {
        value = hydroutil::toStringSeq( stringVal, delim );
        return 0;
    }
}

Ice::StringSeq
getPropertyAsStringSeqWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const Ice::StringSeq & defaultValue, char delim )
{
    std::string stringVal;
    if ( orcaice::getProperty( prop, key, stringVal ) )
        return defaultValue;
    else
        return hydroutil::toStringSeq( stringVal, delim );
}

int
getPropertyAsFrame2d( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Frame2d & value )
{
    std::string stringVal;
    if ( orcaice::getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toFrame2d( stringVal, value );
}

orca::Frame2d
getPropertyAsFrame2dWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Frame2d & defaultValue )
{
    orca::Frame2d value;
    if ( getPropertyAsFrame2d( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

int
getPropertyAsFrame3d( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Frame3d & value )
{
    std::string stringVal;
    if ( orcaice::getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toFrame3d( stringVal, value );
}

orca::Frame3d
getPropertyAsFrame3dWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Frame3d & defaultValue )
{
    orca::Frame3d value;
    if ( getPropertyAsFrame3d( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

int
getPropertyAsSize2d( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Size2d & value )
{
    std::string stringVal;
    if ( orcaice::getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toSize2d( stringVal, value );
}

orca::Size2d
getPropertyAsSize2dWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Size2d & defaultValue )
{
    orca::Size2d value;
    if ( getPropertyAsSize2d( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

int
getPropertyAsSize3d( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Size3d & value )
{
    std::string stringVal;
    if ( orcaice::getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toSize3d( stringVal, value );
    return 0;
}

orca::Size3d
getPropertyAsSize3dWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Size3d & defaultValue )
{
    orca::Size3d value;
    if ( getPropertyAsSize3d( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

int
getPropertyAsCartesianPoint( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::CartesianPoint & value )
{
    std::string stringVal;
    if ( orcaice::getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toCartesianPoint( stringVal, value );
    return 0;
}

orca::CartesianPoint
getPropertyAsCartesianPointWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::CartesianPoint & defaultValue )
{
    orca::CartesianPoint value;
    if ( getPropertyAsCartesianPoint( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

int 
getPropertyAsTimeDuration( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::Time &value )
{
    std::string stringVal;
    if ( orcaice::getProperty( prop, key, stringVal ) )
        return -1;
    else
        return toTimeDuration( stringVal, value );
}

orca::Time
getPropertyAsTimeDurationWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::Time & defaultValue )
{
    orca::Time value;
    if ( getPropertyAsTimeDuration( prop, key, value ) )
        return defaultValue;
    else
        return value;
}

// int
// getPropertyAsImageFormat( const Ice::PropertiesPtr & prop, const ::std::string& key, orca::ImageFormat & value )
// {
//     std::string stringVal;
//     if ( orcaice::getProperty( prop, key, stringVal ) )
//         return -1;
//     else
//         return toImageFormat( stringVal, value );
// }
// 
// orca::ImageFormat 
// getPropertyAsImageFormatWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const orca::ImageFormat & defaultValue )
// {
//     orca::ImageFormat value;
//     if ( getPropertyAsImageFormat( prop, key, value ) )
//         return defaultValue;
//     else
//         return value;
// }

} // namespace
