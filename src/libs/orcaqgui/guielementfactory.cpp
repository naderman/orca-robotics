#include "guielementfactory.h"
#include <iostream>

using namespace std;

namespace orcaqgui {

std::vector<QString> 
GuiElementFactory::supportedElementTypes() const
{
    return supportedElementTypes_;
}

bool 
GuiElementFactory::isSupported( const QString &elementType ) const
{
    for ( unsigned int i=0; i<supportedElementTypes_.size(); ++i ) {
        if ( supportedElementTypes_[i] == elementType ) {
            return true;
        }
    }
    return false;
}

void
GuiElementFactory::addSupportedType( const QString & elementType )
{
    supportedElementTypes_.push_back( elementType );
}


}
