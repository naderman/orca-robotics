/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */


#include "configfileelements.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <hydroutil/stringutils.h> 

using namespace std;

namespace orcaqgui {

    void
    loadGrid( orcaqgemv::GuiElementModel &guiElementModel )
    {
        const QString elementType("Grid");
        QStringList elementDetails;
        elementDetails.push_back("local@global/local");

        guiElementModel.createGuiElement( elementType, elementDetails );
    }
    
    
    void
    loadElementsFromConfigFile( orcaqgemv::GuiElementModel &guiElementModel,
                                const orcaice::Context     &context )
    {
        // create a map of types
        const string typePrefix = context.tag() + ".Config.Element.Type";
        std::map<string,string> typeMap = context.properties()->getPropertiesForPrefix(typePrefix);
        
        // create a map of details
        const string detailsPrefix = context.tag() + ".Config.Element.Detail";
        std::map<string,string> detailMap = context.properties()->getPropertiesForPrefix(detailsPrefix);
        
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
            
            // find the corresponding entry in the detailMap
            map<string,string>::iterator detailMapIt = detailMap.find( context.tag() + ".Config.Element.Detail" + key );
            if ( detailMapIt==detailMap.end() ) 
            {
                ss.str(""); ss << "'Detail' entry with key " << key << " expected. Check your config file!";
                context.tracer().warning( ss.str() );
                continue;
            }
            
            // found an entry
            ss.str(""); ss << "Found Detail entry: " << detailMapIt->second;
            context.tracer().debug( ss.str(), 5 );
            
            // some elements may have multiple interfaces: split details into separate strings
            const char delim = ':';
            Ice::StringSeq seq = hydroutil::toStringSeq( detailMapIt->second, delim );
            
            QStringList elementDetails;
            for ( unsigned int k=0; k<seq.size(); k++ ) {
                elementDetails.push_back(QString(seq[k].c_str()));
            }
            
            // debug output
            for ( int i=0; i<elementDetails.size(); i++ )
            {
                stringstream ss; ss << "Detail of interface " << i << ": " << elementDetails[i].toStdString();
                context.tracer().debug( ss.str(), 5 );
            }
            
            // finally: create the GuiElement
            guiElementModel.createGuiElement( elementType, elementDetails );
        }

    }
  
}

