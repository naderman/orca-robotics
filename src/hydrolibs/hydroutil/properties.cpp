/*
Orca Project: Components for robotics 
             http://orca-robotics.sf.net/
Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
This copy of Orca is licensed to you under the terms described in the
ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <assert.h>
#include <cstdlib>
#include <gbxutilacfr/exceptions.h>
#include "properties.h"
#include "stringutils.h" // for toIntVector(), and toDoubleVector()

using namespace hydroutil;
using namespace std;

Properties::Properties( const Config& config ) :
    config_(config)
{
}

Properties::Properties( const map<string,string>& props, const std::string& removePrefix,
             const Config& config ) :
    config_(config)
{
    if ( removePrefix.empty() ) 
    {
        props_ = props;
    }
    else 
    {
        for ( map<string,string>::const_iterator it=props.begin(); it!=props.end(); ++it ) 
        {
            // check that the prefix exists
            std::string::size_type postPrefix = it->first.find( removePrefix );
            if ( postPrefix == std::string::npos ) {
                props_.insert( make_pair(it->first,it->second) );
            }
            else if ( postPrefix==0 ) {
                props_.insert( make_pair(it->first.substr(removePrefix.size()),it->second) );
            }
            else {
                assert( false && "prefix must be at the beginning of the key string" );
            }
        }
    }
}

bool 
Properties::isDefined( const ::std::string& key )
{
    map<string,string>::const_iterator it = props_.find( key );
    if ( it == props_.end() )
        return false;

    return true;
}

void 
Properties::ensureDefined( const std::string& key )
{
    if ( !isDefined( key ) )
        throw gbxutilacfr::Exception( ERROR_INFO, missingPropertyWarning(key) );
}

int 
Properties::getProperty( const string& key, string &value ) const
{
    map<string,string>::const_iterator it = props_.find( key );
    if ( it == props_.end() )
        return -1;
    
    value = it->second;
    return 0;
}

int 
Properties::getPropertyAsDouble( const string& key, double &value ) const
{
    string stringVal;
    if ( getProperty( key, stringVal ) )
    {
        return -1;
    }
    else
    {
        // atof returns 0.0 if failes to convert, can't detect errors
        //d = atof( stringVal.c_str() );
        
        stringstream ss( stringVal );
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

int 
Properties::getPropertyAsInt( const string& key, int &value ) const
{
    string stringVal;
    if ( getProperty( key, stringVal ) )
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

string 
Properties::getPropertyWithDefault( const string& key, const string &defaultValue ) const
{
    string value;
    if ( getProperty( key, value ) )
    {
        // use default
        value = defaultValue;

        if ( config_.warnDefaultProperty )
            cout << "hydro: Set property to default value: "<<key<<"="<<value<<endl;
    }
    return value;
}

double 
Properties::getPropertyAsDoubleWithDefault( const string& key, const double defaultValue ) const
{
    stringstream ss;
    ss << defaultValue;

    string stringVal = getPropertyWithDefault( key, ss.str() );

    // here it's ok to use atof because we guaranteed to start with a double
    return atof( stringVal.c_str() );
}

int 
Properties::getPropertyAsIntWithDefault( const string& key, const int defaultValue ) const
{
    stringstream ss;
    ss << defaultValue;

    string stringVal = getPropertyWithDefault( key, ss.str() );
    
    // here it's ok to use atoi because we guaranteed to start with an int
    return atoi( stringVal.c_str() );
}

int
Properties::getPropertyAsIntVector( const ::std::string& key, std::vector<int> &value ) const
{
    std::string stringVal;
    if ( getProperty( key, stringVal ) )
        return -1;
    else
        if ( toIntVector( stringVal, value ) )
            throw gbxutilacfr::Exception( ERROR_INFO, "Failed to parse '"+stringVal+"' to a vector of ints" );
    return 0;
}

std::vector<int>
Properties::getPropertyAsIntVectorWithDefault( const ::std::string& key, const std::vector<int>& defaultValue ) const
{
    std::vector<int> value;
    if ( getPropertyAsIntVector( key, value ) ) {
        cout << "hydro: Set property to default value: "<<key<<"=";
        for ( size_t i=0; i<defaultValue.size(); ++i )
            cout << defaultValue.at(i) << " ";
        cout<<endl;
        return defaultValue;
    }
    else
        return value;
}

int
Properties::getPropertyAsDoubleVector( const ::std::string& key, std::vector<double> &value ) const
{
    std::string stringVal;
    if ( getProperty( key, stringVal ) )
        return -1;
    else {
        if ( toDoubleVector( stringVal, value ) )
            throw gbxutilacfr::Exception( ERROR_INFO, "Failed to parse '"+stringVal+"' to a vector of doubles" );
    }
    return 0;
}

std::vector<double>
Properties::getPropertyAsDoubleVectorWithDefault( const ::std::string& key, const std::vector<double>& defaultValue ) const
{
    std::vector<double> value;
    if ( getPropertyAsDoubleVector( key, value ) ) {
        cout << "hydro: Set property to default value: "<<key<<"=";
        for ( size_t i=0; i<defaultValue.size(); ++i )
            cout << defaultValue.at(i) << " ";
        cout<<endl;
        return defaultValue;
    }
    else
        return value;
}

void 
Properties::setProperty( const string& key, const string& value )
{
    // should we use insert() ??
    props_[key] = value;
}

string
Properties::toString() const
{
    stringstream ss;
    ss<<size()<<" properties:"<<endl;
    for ( map<string,string>::const_iterator it=props_.begin(); it!=props_.end(); ++it ) {
        ss << it->first << "=" << it->second << endl;
    }
    return ss.str();
}

std::string 
Properties::missingPropertyWarning( const std::string& key, bool fatal )
{
    if ( fatal )
        return "Cannot continue because property '" + key + "' is not set.";
    else
        return "Property '" + key + "' is not set.";
}
