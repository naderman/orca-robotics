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
                                const orcaice::Context &context )
    {
        // get properties from config file
        string prefix = context.tag() + ".Config.Element";
        int i=0;
        QString elementType;
        QStringList elementDetails;
    
        while (true)
        {
            stringstream key;
            key << prefix << i;
            Ice::StringSeq strOut;
        
            //  Find elementType
            int ret = orcaobj::getPropertyAsStringSeq( context.properties(), key.str()+".Type", strOut );
            if (ret!=0) break;
            elementType = QString(strOut[0].c_str());
        
            //  Find elementDetails
            ret = orcaobj::getPropertyAsStringSeq( context.properties(), key.str()+".Detail", strOut );
            elementDetails.clear();
            if (ret!=0)
            {
                // no detail means special element with no interfaces like Grid
                elementDetails.push_back("local@global/local");
            }
            else
            {
                for ( unsigned int k=0; k<strOut.size(); k++ )
                {
                    elementDetails.push_back(QString(strOut[k].c_str()));
                }
            }

            // Debug output
            cout << "TRACER(configfileelements.cpp): Loading element of type: " << elementType.toStdString() << endl;
            cout << "TRACER(configfileelements.cpp): Loading element with details: ";
            for (int k=0; k<elementDetails.size(); k++)
            {
                cout << elementDetails[k].toStdString() << " ";
            }
            cout << endl;
            // end of debug output
        
            guiElementModel.createGuiElement( elementType, elementDetails );
            i++;
        }
    }

}

