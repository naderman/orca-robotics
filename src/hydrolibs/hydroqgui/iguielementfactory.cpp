/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include "iguielementfactory.h"
#include <iostream>

using namespace std;

namespace hydroqgui {

std::vector<QString> 
IGuiElementFactory::supportedElementTypes() const
{
    return supportedElementTypes_;
}

std::vector<std::string> 
IGuiElementFactory::supportedElementTypesAsStdString() const
{
    std::vector<std::string> types;
    for ( size_t i=0; i<supportedElementTypes_.size(); ++i ) {
        types.push_back( supportedElementTypes_[i].toStdString() );
    }
    return types;
}

bool 
IGuiElementFactory::isSupported( const QString &elementType ) const
{
    for ( unsigned int i=0; i<supportedElementTypes_.size(); ++i ) {
        if ( supportedElementTypes_[i] == elementType ) {
            return true;
        }
    }
    return false;
}

void
IGuiElementFactory::addSupportedType( const QString & elementType )
{
    supportedElementTypes_.push_back( elementType );
}


}
