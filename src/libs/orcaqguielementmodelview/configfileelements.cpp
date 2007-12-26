#include "configfileelements.h"
#include <iostream>
#include <orcaice/orcaice.h>

using namespace std;

namespace orcaqgemv {

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
            int ret = orcaice::getPropertyAsStringSeq( context.properties(), key.str()+".Type", strOut );
            if (ret!=0) break;
            elementType = QString(strOut[0].c_str());
        
            //  Find elementDetails
            ret = orcaice::getPropertyAsStringSeq( context.properties(), key.str()+".Detail", strOut );
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
            cout << "TRACER(component.cpp): Loading element of type: " << elementType.toStdString() << endl;
            cout << "TRACER(component.cpp): Loading element with details: ";
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

