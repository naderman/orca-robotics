/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCADEF_COMPONENT_DEFINITION_H
#define ORCADEF_COMPONENT_DEFINITION_H

#include <string>
#include <vector>

#include <orca/orca.h>

namespace orcadef
{

//
// Notes:
//
// - 'Defaults' are the default values to use in the config files.
// - Optional Definition fields can be left empty.  They'll be filled
//   out with reasonable values when .cfg files are generated.
//


//
// Component Definition: these bits are static for a given component
//
////////////////////////////////////////////////////////////////////////////////

//! Definition of an interface
class InterfaceDef
{
public:
    //! Tag in the configuration file. Required.
    std::string tag;
    //! Interface type. Required.
    std::string type;
};

//! Definition of a provided interface
class ProvidedDef : public InterfaceDef
{
public:
    //! Registered name. Optional. Used as the default interface name.
    std::string name;
};

//! Definition of a required interface
class RequiredDef : public InterfaceDef
{
public:
    //! Stringified proxy for the required interface. Optional. Used as the default proxy name.
    std::string proxy;
//     orca::FQInterfaceName fqname;
};

//! Definition of a configuration option
struct ConfigDef
{
    //! Tag in the configuration file. Required.
    std::string tag;
    //! Value in the configuration file. Required. Used as the default configuration value.
    std::string value;
    //! Comment in the configuration file. Optional.
    std::string comment;
    //! Value type, eg. 'int', 'string', etc. Optional.
    std::string type;
    //! If TRUE, indicates that this configuration option is external to the component, e.g. an Ice option.
    //! Optional. Default is FALSE.
    bool isExternal;
};

/*!
    @brief Holds the information for component definition.

    To load from file use orcadef::parseDefFile.
*/
struct ComponentDef
{
    //! Component definition file name
    std::string              filename;

    //! Default component tag
    std::string              tag;
    //! Default component endpoints
    std::string              endpoints;
    //! Are endpoints non-standard
    bool isSpecialEndpoints;
    //! Top-level comments
    std::vector<std::string> comments;

    //! Provided interfaces.
    std::vector<ProvidedDef> provided;
    //! Required interfaces.
    std::vector<RequiredDef> required;
    //! Component configuration parameters.
    std::vector<ConfigDef>   configs;
    //! External configuration parameters.
    std::vector<ConfigDef> externalConfigs;
};

//
// Component Configuration: these bits are user-edittable.
//
////////////////////////////////////////////////////////////////////////////////

//! Configuration of an interface
class InterfaceCfg
{
public:
    //! Tag in the configuration file.
    std::string tag;
};

//! Configuration of a provided interface
class ProvidedCfg : public InterfaceCfg
{
public:
    //! Registered name
    std::string name;
};

//! Configuration of a required interface
class RequiredCfg : public InterfaceCfg
{
public:
    //! Stringified proxy.
    std::string proxy;
//     orca::FQInterfaceName fqname;
};

//! Configuration option
struct ConfigCfg
{
    //! Tag in the configuration file.
    std::string tag;
    //! Value in the configuration file.
    std::string value;
};

/*!
    @brief Holds the information for component configuration.

    To convert from component definition, use orcadef::defToCfg.
*/
struct ComponentCfg
{
    //! Fully-qualified component name.
    orca::FQComponentName  fqname;
    //! Component executable.
    orca::FQExecutableName fqexecutable;
    //! Adapter's commnication endpoints (e.g. tcp -h xxx -p xxx )
    std::string endpoints;
    //! Provided interfaces.
    std::vector<ProvidedCfg> provided;
    //! Required interfaces.
    std::vector<RequiredCfg> required;
    //! Component configuration parameters.
    std::vector<ConfigCfg> configs;
    //! External configuration parameters.
    std::vector<ConfigCfg> externalConfigs;
};

/*!
    Generates a configuration structure based on definitions and built-in conventions.
    Throws ParseException if the definition structure is ill-formed.
*/
ComponentCfg toCfg( const ComponentDef & def );

/*!
    Use this function to generate a default config file from a component description structure.
    Throws ParseException if the definition structure is ill-formed.
*/
const std::string toCfgText( const ComponentDef & def );

/*!
    Same as orcadef::toCfgText but adds some common properties.
*/
const std::string toCfgTextWithHeaders( const ComponentDef &def, const std::vector<std::string> & headers );

/*!
    Generates a default descriptor file from a component definition structure.
    Throws ParseException if the definition structure is ill-formed.
*/
const std::string toXmlText( const ComponentDef & def, bool withTemplate=false );

/*!
    Generates a templated descriptor file from a component definition structure.
    Throws ParseException if the definition structure is ill-formed.
*/
const std::string toXmlTemplateText( const ComponentDef & def );

/*!
    Reverses required and provided interfaces. The output file describes a component which 
    provides all the interfaces which the original component requires and vice versa. The input
    configuration parameters are discarded because they don't apply to the output component.
*/
ComponentDef invertDef( const ComponentDef & def, const std::string & invTag );

/*!
    Generates a string in the format suitable for writing to a file.
    Throws ParseException if the definition structure is ill-formed.
*/
const std::string toDefText( const ComponentDef & def );

// Debugging functions: see the entire contents of the ComponentDef.
const std::string toString( const ProvidedDef &def );
const std::string toString( const RequiredDef &def );
const std::string toString( const ConfigDef &def );
const std::string toString( const ComponentDef &def );

// Debugging functions: see the entire contents of the ComponentCfg.
const std::string toString( const ProvidedCfg &c );
const std::string toString( const RequiredCfg &c );
const std::string toString( const ConfigCfg &c );
const std::string toString( const ComponentCfg &c );

} // namespace

#endif
