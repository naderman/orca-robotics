#include "guielementfactory.h"
#include <iostream>

using namespace std;

namespace orcaqgui {

std::vector<QStringList> 
GuiElementFactory::supportedInterfaceIds() const
{
    return supportedInterfaceIds_;
}

bool 
GuiElementFactory::isSupported( const QStringList &interfaceIds ) const
{
    for ( unsigned int i=0; i<supportedInterfaceIds_.size(); ++i ) {
        if ( supportedInterfaceIds_[i] == interfaceIds ) {
            return true;
        }
    }
    return false;
}

void
GuiElementFactory::addSupportedType( const QStringList & interfaceType )
{
    supportedInterfaceIds_.push_back( interfaceType );
}


}
