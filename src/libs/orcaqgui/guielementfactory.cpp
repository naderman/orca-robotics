#include "guielementfactory.h"
#include <iostream>

using namespace std;

namespace orcaqgui {

std::vector<QString> 
GuiElementFactory::supportedInterfaceIds() const
{
    return supportedInterfaceIds_;
}

bool 
GuiElementFactory::isSupported( const QString &interfaceId ) const
{
    for ( uint i=0; i<supportedInterfaceIds_.size(); ++i ) {
        if ( supportedInterfaceIds_[i] == interfaceId ) {
            return true;
        }
    }
    return false;
}

void
GuiElementFactory::addSupportedType( const QString & interfaceType )
{
    supportedInterfaceIds_.push_back( interfaceType );
}


}
