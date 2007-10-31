/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAQUML_MODEL_H
#define ORCAQUML_MODEL_H

#include <vector>
#include <string>
#include <orcadef/componentdefinition.h>

// Different derived QGraphicsItem's are identified by user defined types
#define RTTI_PROVIDED_INTERFACE 6666
#define RTTI_REQUIRED_INTERFACE 6667

namespace orcaquml
{

struct InterfaceGeom
{
    double x;
    double y;
};

//! On-screen geometry of a component. Used to save and reload on-screen configuration.
struct ComponentGeom 
{
    int  id;
    double left;
    double top;
    bool isHighlighted;
    std::vector<InterfaceGeom> provided;
    std::vector<InterfaceGeom> required;
};

//! A class to hold all relevant component information.
class ComponentModel
{
public:

    ComponentModel( const orcadef::ComponentDef &def,
                    const orcadef::ComponentCfg &cfg,
                    const ComponentGeom         &geom );
    ComponentModel( const orcadef::ComponentDef &def,
                    const orcadef::ComponentCfg &cfg );
    ComponentModel( const orcadef::ComponentDef &def );

    orcadef::ComponentDef def;
    orcadef::ComponentCfg cfg;
    ComponentGeom         geom;

private:
    void allocateGeomSpace();
};

/*! 
    Currently, a system is simply a collection of components.
    Should also add information about the links.
*/ 
typedef std::vector<ComponentModel*> SystemModel;

const std::string toString( const SystemModel & );
const std::string toString( const ComponentModel & );
const std::string toString( const ComponentGeom & );
const std::string toString( const InterfaceGeom & );

}

#endif
