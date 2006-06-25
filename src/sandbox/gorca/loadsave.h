/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef GORCA_LOADSAVE_H
#define GORCA_LOADSAVE_H

#include <qorcauml/qorcacomponent.h>
#include <orcadef/componentdefinition.h>
#include <iostream>

#include <qorcauml/qorcacanvas.h>
#include <qorcauml/qorcamodel.h>

namespace gorca_loadsave {

// Functions for loading and saving projects

//     const std::string toString( QorcaComponent &c );

//     void toFile(   QorcaComponent &c, const std::string &fileName );
//     void fromFile( QorcaComponent &c, const std::string &fileName );

    void toFile(   QorcaModel &c, const std::string &fileName );
    void fromFile( QorcaModel &c, const std::string &fileName );

std::ostream &operator<<( std::ostream &s, const QorcaModel &canvas );
std::istream &operator>>( std::istream &s, QorcaModel &canvas );

std::ostream &operator<<( std::ostream &s, const ComponentModel &component );
std::istream &operator>>( std::istream &s, ComponentModel &component );

std::ostream &operator<<( std::ostream &s, const ComponentGeom &g );
std::istream &operator>>( std::istream &s, ComponentGeom &g );

std::ostream &operator<<( std::ostream &s, const InterfaceGeom &g );
std::istream &operator>>( std::istream &s, InterfaceGeom &g );

//
// ComponentDef
//

std::ostream &operator<<( std::ostream &s, const orcadef::ComponentDef &component );
std::istream &operator>>( std::istream &s, orcadef::ComponentDef &component );

std::ostream &operator<<( std::ostream &s, const orcadef::ProvidedDef &p );
std::istream &operator>>( std::istream &s, orcadef::ProvidedDef &p );

std::ostream &operator<<( std::ostream &s, const orcadef::RequiredDef &r );
std::istream &operator>>( std::istream &s, orcadef::RequiredDef &r );

std::ostream &operator<<( std::ostream &s, const orcadef::ConfigDef &c );
std::istream &operator>>( std::istream &s, orcadef::ConfigDef &c );

//
// ComponentCfg
//
std::ostream &operator<<( std::ostream &s, const orcadef::ComponentCfg &c );
std::istream &operator>>( std::istream &s, orcadef::ComponentCfg &c );

std::ostream &operator<<( std::ostream &s, const orcadef::ConfigCfg &c );
std::istream &operator>>( std::istream &s, orcadef::ConfigCfg &c );

std::ostream &operator<<( std::ostream &s, const orcadef::RequiredCfg &r );
std::istream &operator>>( std::istream &s, orcadef::RequiredCfg &r );

std::ostream &operator<<( std::ostream &s, const orcadef::ProvidedCfg &c );
std::istream &operator>>( std::istream &s, orcadef::ProvidedCfg &c );

//
// orca::
//
std::ostream &operator<<( std::ostream &s, const orca::FQInterfaceName &i );
std::istream &operator>>( std::istream &s, orca::FQInterfaceName &i );

std::ostream &operator<<( std::ostream &s, const orca::FQExecutableName &e );
std::istream &operator>>( std::istream &s, orca::FQExecutableName &e );

std::ostream &operator<<( std::ostream &s, const orca::FQComponentName &c );
std::istream &operator>>( std::istream &s, orca::FQComponentName &c );

}

#endif
