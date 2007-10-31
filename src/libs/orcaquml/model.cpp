/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <sstream>

#include "model.h"

using namespace std;
using namespace orcaquml;

ComponentModel::ComponentModel( const orcadef::ComponentDef &def,
                                const orcadef::ComponentCfg &cfg,
                                const ComponentGeom             &geom )
{
    this->def  = def;
    this->cfg  = cfg;
    this->geom = geom;
}

ComponentModel::ComponentModel( const orcadef::ComponentDef &def,
                                const orcadef::ComponentCfg &cfg )
{
    this->def = def;
    this->cfg = cfg;

    allocateGeomSpace();
}

ComponentModel::ComponentModel( const orcadef::ComponentDef &def )
{
    this->def = def;
    this->cfg = orcadef::toCfg( def );

    allocateGeomSpace();
}

void
ComponentModel::allocateGeomSpace()
{
    // Quick sanity checks
    assert( def.provided.size() == cfg.provided.size() );
    assert( def.required.size() == cfg.required.size() );
    assert( def.configs.size() == cfg.configs.size() );

    geom.provided.resize( def.provided.size() );
    geom.required.resize( def.required.size() );
}

const std::string 
orcaquml::toString( const SystemModel &m )
{
    stringstream ss;
    ss<<"System of "<<m.size()<<" components"<<endl;
    for ( unsigned int i=0; i < m.size(); i++ )
    {
        ss << "Component " << i << ":" << endl;
        ss << toString(*(m[i]));
    }
    return ss.str();
}

const std::string 
orcaquml::toString( const ComponentModel &m )
{
    stringstream ss;

    ss << "Def:" << endl;
    ss << toString( m.def );
    ss << "Cfg:" << endl;
    ss << toString( m.cfg );
    ss << "Geom:" << endl;
    ss << toString( m.geom );

    return ss.str();
}

const std::string 
orcaquml::toString( const ComponentGeom &g )
{
    stringstream ss;
    ss << "  ID:   " << g.id << endl;
    ss << "  left: " << g.left << endl;
    ss << "  top : " << g.top << endl;

    if ( g.provided.size() > 0 )
    {
        ss << "  Provided Interface Geometry:" << endl;
        for ( unsigned int i=0; i < g.provided.size(); i++ )
        {
            ss << toString( g.provided[i] );
        }
    }
    if ( g.required.size() > 0 )
    {
        ss << "  Required Interface Geometry:" << endl;
        for ( unsigned int i=0; i < g.required.size(); i++ )
        {
            ss << toString( g.required[i] );
        }
    }
    return ss.str();
}

const std::string 
orcaquml::toString( const InterfaceGeom &g )
{
    stringstream ss;

    ss << "    x: " << g.x << endl;
    ss << "    y: " << g.y << endl;

    return ss.str();
}
