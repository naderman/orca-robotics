/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCADEF_DEF_PARSER_H
#define ORCADEF_DEF_PARSER_H

#include <Ice/Ice.h>
#include <string>
#include <orcadef/componentdefinition.h>

namespace orcadef {

/*!
    Opens the component definition file and parses it.

    May throw exceptions:
    - orcadef::FileException:   if the def file can't be opened
    - orcadef::ParseException   if the def file is badly-formed.
    
    @see defToCfg
*/
void parseDefFile( const std::string &filename, ComponentDef &def );

// THIS CLASS IS NOT PART OF THE PUBLIC API. USE orcadef::parseDefFile()
/*
    @brief Parses .def files.
    
    @author Alex Brooks
*/
class DefParser
{

public: 

    /*
        Opens the .def (component definition) file and parses it.
    
        May throw exceptions:
        - orcadef::FileException:   if the def file can't be opened
        - orcadef::ParseException   if the def file is badly-formed.
        
        To generate .def files, just follow this example:
        @include src/utils/orcadef/def_test_input.def
    */
    DefParser( std::string filename, orcadef::ComponentDef &def );
    ~DefParser();

    // Access
    ///////////
    const ComponentDef &componentDefinition() const { return def_; }

private: 

    void parse();

    void listAllPropertiesForPrefix( std::string prefix );
    void initValidTypes();

    void getComponentComments();
    // interfaceType="Provides" or "Requires" (no ".")
    void getInterfaceTags( const std::string        &interfaceType, 
                           std::vector<std::string> &tagList);
    void getProvidedInterfaceDetails( const std::vector<std::string> &tags, 
                                      std::vector<ProvidedDef>       &p );
    void getRequiredInterfaceDetails( const std::vector<std::string> &tags, 
                                      std::vector<RequiredDef>       &p );
    
    // configType="Config" or "ExternaConfig" (no ".")
    void getConfigTags( const std::string        &configType, 
                        std::vector<std::string> &configTags );
    // configType="Config" or "ExternaConfig" (no ".")
    void getConfigDetails( const std::string        &configType,
                           const std::vector<std::string>  &tags,
                           std::vector<ConfigDef>          &p );

    // Throws an exception if it's not found.
    std::string forceGetProperty( std::string key );

    ComponentDef &def_;

    Ice::PropertiesPtr props_;

    // Constants
    //////////////

    Ice::StringSeq validConfigTypes_;

    int verbose_;
};

}

#endif
