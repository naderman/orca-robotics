/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
  
#include <orcaice/proputils.h>
#include <hydroutil/stringutils.h>

#include "defparser.h"
#include "exceptions.h"

using namespace std;
using namespace Ice;

namespace orcadef {


void
parseDefFile( const std::string &filename, ComponentDef &def )
{
    DefParser defParser( filename, def );
}


////////////////////////////////////////////////////////////////////////////////
//                          Utility Functions                                 //
////////////////////////////////////////////////////////////////////////////////
bool
containsString( const Ice::StringSeq &strSeq, const std::string &aString )
{
    Ice::StringSeq::const_iterator it = find( strSeq.begin(),
                                              strSeq.end(),
                                              aString );
    if ( it == strSeq.end() )
    {
        return false;
    }
    return true;
}

void
pushBackIfNew( Ice::StringSeq &thingSeq, const std::string &thing )
{
    if ( !containsString( thingSeq, thing ) )
    {
        thingSeq.push_back( thing );
    }
}

void printStringSeq( Ice::StringSeq &s )
{
    for ( unsigned int i=0; i < s.size(); i++ )
    {
        cout << s[i] << ", ";
    }
    cout << endl;
}

////////////////////////////////////////////////////////////////////////////////

DefParser::DefParser( std::string filename, ComponentDef &def )
    : def_(def),
      props_(Ice::createProperties()),
      verbose_(0)
{
    try {
        props_->load(filename);
    }
    catch ( Ice::SyscallException & )
    {
        throw FileException( ERROR_INFO, "failed to load file '" + filename + "'" );
    }

    // remember which file was loaded
    def_.filename = filename;

    initValidTypes();
    parse();
}

DefParser::~DefParser()
{
}

void
DefParser::initValidTypes()
{
    validConfigTypes_.push_back( "string" );
    validConfigTypes_.push_back( "int" );
    validConfigTypes_.push_back( "double" );
    validConfigTypes_.push_back( "angle" );
}

string
DefParser::forceGetProperty( std::string propName )
{
    string strVal;
    strVal = props_->getProperty( propName );
    if ( strVal == "" )
    {
        throw ParseException( ERROR_INFO, "Required property '" + propName + "' not found." );
    }
    return strVal;
}

void
DefParser::parse()
{
    if ( verbose_ > 1 )
    {
        cout<<"defparser.cpp: parse(): all properties: " << endl;
        listAllPropertiesForPrefix("");
    }

    // Component
    def_.tag = forceGetProperty( "DefaultComponentTag" );

    // get endpoints and remember if they were provided or we are using defaults
    def_.endpoints = props_->getPropertyWithDefault( "DefaultComponentEndpoints", "tcp -t 5000" );
    def_.isSpecialEndpoints = ( def_.endpoints != "tcp -t 5000" );

    getComponentComments();

    // Provided Interfaces
    std::vector<string> providedInterfaceTags;
    getInterfaceTags("Provides", providedInterfaceTags);
    getProvidedInterfaceDetails( providedInterfaceTags, def_.provided );

    // Required Interfaces
    std::vector<string> requiredInterfaceTags;
    getInterfaceTags("Requires", requiredInterfaceTags);
    getRequiredInterfaceDetails( requiredInterfaceTags, def_.required );

    // Component Configuration Parameters
    std::vector<string> configTags;
    getConfigTags( "Config", configTags );
    getConfigDetails( "Config", configTags, def_.configs );

    // External Configuration Parameters
    std::vector<string> externalConfigTags;
    getConfigTags( "ExternalConfig", externalConfigTags );
    getConfigDetails( "ExternalConfig", externalConfigTags, def_.externalConfigs );
}

void
DefParser::getProvidedInterfaceDetails( const vector<string>     &tags, 
                                        std::vector<ProvidedDef> &p )
{
    p.resize( tags.size() );
    for ( unsigned int i=0; i < tags.size(); ++i )
    {
        p[i].tag  = tags[i];
        p[i].type = forceGetProperty(    "Provides." + tags[i] + ".Type" );
        p[i].name = props_->getProperty( "Provides." + tags[i] + ".DefaultName" );
    }
}

void
DefParser::getRequiredInterfaceDetails( const vector<string>     &tags, 
                                        std::vector<RequiredDef> &p )
{
    p.resize( tags.size() );
    for ( unsigned int i=0; i < tags.size(); ++i )
    {
        p[i].tag   = tags[i];
        p[i].type  = forceGetProperty(    "Requires." + tags[i] + ".Type" );
        p[i].proxy = props_->getProperty( "Requires." + tags[i] + ".DefaultProxy" );
    }
}

void
DefParser::getComponentComments()
{
    PropertyDict p = props_->getPropertiesForPrefix("Component.Comment");
    for ( PropertyDict::iterator i = p.begin(); i != p.end(); ++i )
    {
        def_.comments.push_back(i->second);
    }
}

void
DefParser::getConfigTags(const std::string &configType, std::vector<string> &configTags )
{
//    listAllPropertiesForPrefix(configType);

    const string tailString=".Default";

    PropertyDict p = props_->getPropertiesForPrefix(configType);
    for ( PropertyDict::iterator i = p.begin();
          i != p.end();
          i++ )
    {
        // Strip off the leading 'Config.'
        string configPropStr = i->first.substr( i->first.find('.')+1, i->first.size()-1 );

        // Look for the string at the end of the property name
        int tailStart = configPropStr.rfind( tailString );
        if ( tailStart == -1 ) 
            continue;
        if ( (configPropStr.size() - tailStart) != tailString.size() ) 
            continue;

        // strip off the trailing string
        string cfg = configPropStr.substr( 0, tailStart );
        pushBackIfNew( configTags, cfg );
    }
}

void
DefParser::getConfigDetails( const std::string      &configType,
                             const vector<string>   &tags,
                             std::vector<ConfigDef> &p )
{
    p.resize( tags.size() );
    for ( unsigned int i=0; i < tags.size(); ++i )
    {
        p[i].tag     = tags[i];
        // this one is required, so we force it.
        p[i].value   = forceGetProperty( configType + "." + tags[i] + ".Default" );
        // the rest are optional
        p[i].comment = props_->getProperty( configType + "." + tags[i] + ".Comment" );
        p[i].type    = props_->getProperty( configType + "." + tags[i] + ".Type" );
        // if config type is supplied, make sure it's on the list of allowed types.
        if ( p[i].type != "" )
        {
            if ( !containsString( validConfigTypes_, p[i].type ) )
            {
                throw ParseException( ERROR_INFO, "Invalid type for config parameter '" + tags[i] + "': '" + p[i].type + "'" );
            }
        }
        //
        // This behavior is OBSOLETE and will be removed soon.
        //
        p[i].isExternal = props_->getPropertyAsInt( configType + "." + tags[i] + ".External" );
    }
}

void
DefParser::getInterfaceTags(const std::string &interfaceType, std::vector<string> &tagList)
{
    // listAllPropertiesForPrefix(interfaceType);

    PropertyDict p = props_->getPropertiesForPrefix(interfaceType);
    for ( PropertyDict::iterator i = p.begin();
          i != p.end();
          i++ )
    {
        vector<string> s = hydroutil::toStringSeq( i->first, '.' );
        assert(s[0] == interfaceType);
        
        string tag = s[1];
        pushBackIfNew( tagList, tag );
    }
}

void
DefParser::listAllPropertiesForPrefix( std::string prefix )
{
    // cout<<"TRACE(defparser.cpp): All Properties for prefix '" << prefix << "': " << endl;

    PropertyDict p = props_->getPropertiesForPrefix(prefix);
    for ( PropertyDict::iterator i = p.begin();
          i != p.end();
          i++ )
    {
        cout<<"        " << i->first << " -> " << i->second << endl;
    }
}

} // namespace
