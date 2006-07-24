/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef GORCA_FILE_FUNCTIONS_H
#define GORCA_FILE_FUNCTIONS_H

#include <orcadef/componentdefinition.h>
#include <qorcauml/qorcacomponent.h>

namespace gorca
{
#if 0
std::string toString( const orcadef::ComponentDef & obj )
{
    std::ostringstream os;

    os << obj.tag << "\n";

    return os.str();
}

std::string toString( const QorcaComponent::Geometry & obj )
{
    std::ostringstream os;

    os << obj.id << "\n"
       << obj.left << "\n"
       << obj.top << "\n";
    return os.str();
}
#endif
}

#endif
