/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */


#include "configfileelements.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <hydroutil/stringutils.h> 
#include <hydroutil/sysutils.h>

using namespace std;

namespace orcaqgui 
{
    
    namespace
    {
        QString extractPlatformName( const QString &elementDescription )
        {
            QString platformName;
        
            platformName = elementDescription.section('@',1,1);
            platformName = platformName.section('/',0,0);
        
            // replace local with our host name
            if (platformName == "local")
                platformName = QString(hydroutil::getHostname().c_str());

            return platformName;
        }
        
        QString extractUniqueId( const QString &elementDescription )
        {
            // replace local with the resolved platform name 
            
            QString platformName = extractPlatformName(elementDescription);
            QString ifaceName = elementDescription.section('@',0,0);         
            QString componentName = elementDescription.section('/',1,1);
            
            QString uniqueId = ifaceName + '@' + platformName + '/' + componentName;
            
            return uniqueId;
        }
    }

    hydroqguielementutil::GuiElement*
    loadGrid( orcaqgemv::GuiElementModel &guiElementModel )
    {
        const QString elementType("Grid");
        const QString elementDescription("Global Map Grid");
        const QString platformName("global");
        const QString uniqueId("Global Map Grid");

        return guiElementModel.createGuiElement( elementType, elementDescription, platformName, uniqueId );
    }
    
    
    void
    loadElementsFromConfigFile( orcaqgemv::GuiElementModel &guiElementModel,
                                const orcaice::Context     &context )
    {
        // create a map of types
        const string typePrefix = context.tag() + ".Config.Element.Type";
        std::map<string,string> typeMap = context.properties()->getPropertiesForPrefix(typePrefix);
        
        // create a map of descriptions
        const string descriptionPrefix = context.tag() + ".Config.Element.Description";
        std::map<string,string> descriptionMap = context.properties()->getPropertiesForPrefix(descriptionPrefix);
        
        // debug output
        for ( map<string,string>::iterator it=typeMap.begin(); it!=typeMap.end(); ++it )
        {
            stringstream ss;
            ss << "it->first: " << it->first << endl;
            ss << "it->second: " << it->second << endl;  
            context.tracer().debug( ss.str(), 5 );
        }
        
        // create elements based on Type entries
        for ( map<string,string>::iterator it = typeMap.begin(); it != typeMap.end(); ++it )
        {
            // extract the key: e.g. for Config.Element.TypeXX the key is XX
            QString elementType( it->second.c_str() );
            string key = it->first.substr( typePrefix.size() );
            stringstream ss; ss << "Extracted key is: " << key;
            context.tracer().debug( ss.str(), 5 );
            
            // find the corresponding entry in the descriptionMap
            map<string,string>::iterator descriptionMapIt = descriptionMap.find( context.tag() + ".Config.Element.Description" + key );
            if ( descriptionMapIt==descriptionMap.end() ) 
            {
                ss.str(""); ss << "'Description' entry with key " << key << " expected. Check your config file!";
                context.tracer().warning( ss.str() );
                continue;
            }
            
            // found an entry
            ss.str(""); ss << "Found Description entry: " << descriptionMapIt->second;
            context.tracer().debug( ss.str(), 5 );
            
            //
            // assemble all pieces to create the GUI element
            //
            QString elementDescription( descriptionMapIt->second.c_str() );
            QString uniqueId = extractUniqueId( elementDescription );
            QString platformName = extractPlatformName( elementDescription );
            
            guiElementModel.createGuiElement( elementType, elementDescription, platformName, uniqueId );
        }

    }
}

