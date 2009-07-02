/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

bool isPropertyNotNull( const Ice::PropertiesPtr &props, const string& key )
{
    return !props->getProperty( key ).empty();
}

void ensurePropertyNotNull( const Ice::PropertiesPtr &props, const std::string& key )
{
  if ( !isPropertyNotNull( props, key ) )
    throw gbxutilacfr::Exception( ERROR_INFO, warnMissingProperty(key) );
}

// note: the standard tracer already exists, if we pass communicator& into here, we could use it here.
int getProperty( const Ice::PropertiesPtr &props, const string& key, std::string &value )
{
    value = props->getProperty( key );
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
getPropertyWithDefault( const Ice::PropertiesPtr &props, const string& key, const std::string &defaultValue )
{
    std::string value;
    if ( getProperty( props, key, value ) )
    {
        // use default
        value = defaultValue;

        if ( props->getPropertyAsIntWithDefault( "Orca.Warn.DefaultProperty", 1 ) ) {
            initTracerInfo( "Set property to default value: "+key+"="+value );
        }
    }
    return value;
}

int
getPropertyAsDouble( const Ice::PropertiesPtr& props, const string& key, double &value )
{
    std::string stringVal;
    if ( getProperty( props, key, stringVal ) )
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

double
getPropertyAsDoubleWithDefault( const Ice::PropertiesPtr& props, const string& key, double defaultValue )
{
    std::stringstream ssDef;
    ssDef << defaultValue;

    std::string stringVal = getPropertyWithDefault( props, key, ssDef.str() );

    // here it's ok to use atof because we guaranteed to start with a double
    return (double)atof( stringVal.c_str() );
}

int
getPropertyAsInt( const Ice::PropertiesPtr& props, const string& key, int &value )
{
    std::string stringVal;
    if ( getProperty( props, key, stringVal ) )
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
getPropertyAsIntWithDefault( const Ice::PropertiesPtr& props, const string& key, int defaultValue )
{
    std::stringstream ssDef;
    ssDef << defaultValue;

    std::string stringVal = getPropertyWithDefault( props, key, ssDef.str() );
    
    // here it's ok to use atoi because we guaranteed to start with an int
    return atoi( stringVal.c_str() );
}

int
getPropertyAsIntVector( const Ice::PropertiesPtr& props, const string& key, std::vector<int>& value )
{
    std::string stringVal;
    if ( getProperty( props, key, stringVal ) )
        return -1;
    else
        if ( hydroutil::toIntVector( stringVal, value ) )
            throw gbxutilacfr::Exception( ERROR_INFO, "Failed to parse '"+stringVal+"' to a vector of ints" );

    return 0;
}

std::vector<int>
getPropertyAsIntVectorWithDefault( const Ice::PropertiesPtr& props, const string& key, const std::vector<int>& defaultValue )
{
    std::vector<int> value;
    if ( getPropertyAsIntVector( props, key, value ) )
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
getPropertyAsDoubleVector( const Ice::PropertiesPtr& props, const string& key, std::vector<double>& value )
{
    std::string stringVal;
    if ( getProperty( props, key, stringVal ) )
        return -1;
    else
        if ( hydroutil::toDoubleVector( stringVal, value ) )
            throw gbxutilacfr::Exception( ERROR_INFO, "Failed to parse '"+stringVal+"' to a vector of doubles" );
 
    return 0;
}

std::vector<double>
getPropertyAsDoubleVectorWithDefault( const Ice::PropertiesPtr& props, const string& key, const std::vector<double>& defaultValue )
{
    std::vector<double> value;
    if ( getPropertyAsDoubleVector( props, key, value ) )
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
getPropertyAsStringVector( const Ice::PropertiesPtr& props, const string& key, vector<string>& value, char delim )
{
    std::string stringVal;
    if ( orcaice::getProperty( props, key, stringVal ) ) {
        return -1;
    }
    else {
        value = hydroutil::toStringSeq( stringVal, delim );
        return 0;
    }
}

vector<string>
getPropertyAsStringVectorWithDefault( const Ice::PropertiesPtr& props, const string& key, const vector<string>& defaultValue, char delim )
{
    std::string stringVal;
    if ( orcaice::getProperty( props, key, stringVal ) )
        return defaultValue;
    else
        return hydroutil::toStringSeq( stringVal, delim );
}

std::string 
warnMissingProperty( const std::string& props, bool fatal )
{
    if ( fatal )
        return "Cannot continue because property '" + props + "' is not defined or is null.";
    else
        return "Property '" + props + "' is not defined or is null.";
}

std::string 
warnMissingPropertyWithDefault( const std::string& props, const std::string& defaultValue )
{
    return warnMissingProperty(props) + " Using default value '" + defaultValue + "'.";
}

std::string 
warnMissingProperty( const std::string& info, const std::string& props, bool fatal )
{
    if ( fatal )
        return "Cannot continue, missing " + info + " because property '" + props + "' is not defined or is null.";
    else
        return "Missing " + info + " because property '" + props + "' is not defined or is null.";
}

std::string 
warnMissingPropertyWithDefault( const std::string& info, const std::string& props, const std::string& defaultValue )
{
    return warnMissingProperty(info,props) + " Using default value '" + defaultValue + "'.";
}

} // namespace
