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
    
#include <hydroutil/stringutils.h>    // for toStringSeq(), toIntVector(), and toDoubleVector()
#include <gbxutilacfr/mathdefs.h>           // for DEG2RAD_RATIO

#include "proputils.h"
#include "configutils.h"
#include "exceptions.h"
#include "printutils.h"       // initTracerInfo()

using namespace std;

namespace orcaice
{

// note: the standard tracer already exists, if we pass communicator& into here, we could use it here.
int
getProperty( const Ice::PropertiesPtr &prop, const ::std::string& key, std::string &value )
{
    value = prop->getProperty( key );
    if ( value == "" )
    {
        // property not found
        return -1;
    }
    else
    {
        //initTracerInfo( "using configured value '"+value+"' for '"+key+"'");
        return 0;
    }
}

std::string
getPropertyWithDefault( const Ice::PropertiesPtr &prop, const ::std::string& key, const std::string &defaultValue )
{
    std::string value;
    if ( getProperty( prop, key, value ) )
    {
        // use default
        value = defaultValue;

        if ( prop->getPropertyAsIntWithDefault( "Orca.Warn.DefaultProperty", 1 ) ) {
            initTracerInfo( "Set property to default value: "+key+"="+value );
        }
    }
    return value;
}

int
getPropertyAsDouble( const Ice::PropertiesPtr & prop, const ::std::string& key, Ice::Double &value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
    {
        return -1;
    }
    else
    {
        // atof returns 0.0 if failes to convert, can't detect errors
        //d = atof( stringVal.c_str() );
        
        std::istringstream ss( stringVal );
        double d;
        ss >> d;
        if ( !ss )
            throw gbxutilacfr::Exception( ERROR_INFO, "Failed to parse '"+stringVal+"' to a double" );

        // check that there's nothing left (except white space)
        char test = 'a'; // set this char to something something valgrind doesn't complain
        ss >> test;
        if ( test!='#' && !ss.eof() )
            throw gbxutilacfr::Exception( ERROR_INFO, "Failed to parse '"+stringVal+"' to an double" );

        value = d;
    }
    return 0;
}

Ice::Double
getPropertyAsDoubleWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, Ice::Double defaultValue )
{
    std::stringstream ssDef;
    ssDef << defaultValue;

    std::string stringVal = getPropertyWithDefault( prop, key, ssDef.str() );

    // here it's ok to use atof because we guaranteed to start with a double
    return (Ice::Double)atof( stringVal.c_str() );
}

int
getPropertyAsInt( const Ice::PropertiesPtr & prop, const ::std::string& key, int &value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
    {
        return -1;
    }
    else
    {
        // atoi returns 0 if failes to convert, can't detect errors
        //value = atoi( stringVal.c_str() );
        
        std::istringstream ss( stringVal );
        int i;
        ss >> i;
        if ( !ss )
            throw gbxutilacfr::Exception( ERROR_INFO, "Failed to parse '"+stringVal+"' to an int" );

        // check that there's nothing left (except white space)
        char test = 'a'; // set this char to something something valgrind doesn't complain
        ss >> test;
        if ( test!='#' && !ss.eof() )
            throw gbxutilacfr::Exception( ERROR_INFO, "Failed to parse '"+stringVal+"' to an int" );

        value = i;
    }
    return 0;
}

int
getPropertyAsIntWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, int defaultValue )
{
    std::stringstream ssDef;
    ssDef << defaultValue;

    std::string stringVal = getPropertyWithDefault( prop, key, ssDef.str() );
    
    // here it's ok to use atoi because we guaranteed to start with an int
    return atoi( stringVal.c_str() );
}

int
getPropertyAsIntVector( const Ice::PropertiesPtr & prop, const ::std::string& key, std::vector<int> & value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
        return -1;
    else
        if ( hydroutil::toIntVector( stringVal, value ) )
            throw gbxutilacfr::Exception( ERROR_INFO, "Failed to parse '"+stringVal+"' to a vector of ints" );

    return 0;
}

std::vector<int>
getPropertyAsIntVectorWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const std::vector<int>& defaultValue )
{
    std::vector<int> value;
    if ( getPropertyAsIntVector( prop, key, value ) )
    {
        std::ostringstream s;
        for ( unsigned int i=0; i<defaultValue.size(); i++ )
        {
            s << defaultValue.at(i) << " ";
        }
        
        initTracerInfo( "Set property to default value: "+key+"="+s.str() );
        
        return defaultValue;
    }
    else
    {
        return value;
    }
}

int
getPropertyAsDoubleVector( const Ice::PropertiesPtr & prop, const ::std::string& key, std::vector<double> & value )
{
    std::string stringVal;
    if ( getProperty( prop, key, stringVal ) )
        return -1;
    else
        if ( hydroutil::toDoubleVector( stringVal, value ) )
            throw gbxutilacfr::Exception( ERROR_INFO, "Failed to parse '"+stringVal+"' to a vector of doubles" );
 
    return 0;
}

std::vector<double>
getPropertyAsDoubleVectorWithDefault( const Ice::PropertiesPtr & prop, const ::std::string& key, const std::vector<double>& defaultValue )
{
    std::vector<double> value;
    if ( getPropertyAsDoubleVector( prop, key, value ) )
    {
        std::ostringstream s;
        for ( unsigned int i=0; i<defaultValue.size(); i++ )
        {
            s << defaultValue.at(i) << " ";
        }
        
        initTracerInfo( "Set property to default value: "+key+"="+s.str() );
        return defaultValue;
    }
    else
    {
        return value;
    }
}


std::string 
warnMissingProperty( const std::string & prop )
{
    return "Property '" + prop + "' is not set.";
}

std::string 
warnMissingPropertyWithDefault( const std::string & prop, const std::string & defaultValue )
{
    return warnMissingProperty(prop) + " Using default value '" + defaultValue + "'.";
}

std::string 
warnMissingProperty( const std::string & info, const std::string & prop )
{
    return "Missing " + info + " because property '" + prop + "' is not set.";
}

std::string 
warnMissingPropertyWithDefault( const std::string & info, const std::string & prop, const std::string & defaultValue )
{
    return warnMissingProperty(info,prop) + " Using default value '" + defaultValue + "'.";
}

} // namespace
