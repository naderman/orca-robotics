/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "loadsave.h"

#include <string>
#include <fstream>
#include <sstream>
#include <assert.h>

#include <orcaice/orcaice.h>

using namespace std;
using namespace orcadef;

namespace gorca_loadsave {

/////////////////////////// UTILITY FUNCTIONS //////////////////////////////////

void
saveString( std::ostream &s, const std::string &str )
{
    s << str << endl;;
}

void
loadString( std::istream &s, std::string &str )
{
    // Put the entire line into this string.
    std::getline( s, str );
//    s >> str;
//    cout<<"  TRACE(loadsave.cpp): loaded string: " << str << endl;
}

template<class structType>
std::ostream &
saveVector( std::ostream &s, const std::vector<structType> &v, const std::string &id )
{
    s << "-- start of vector --" << endl;
    saveString( s, id );
    s << v.size() << std::endl;
    for ( unsigned int i=0; i < v.size(); i++ )
    {
        s << v[i] << std::endl;
    }
    s << "-- end of vector --" << endl;
    return s;
}

template<class structType>
std::istream &
loadVector( std::istream &s, std::vector<structType> &v, const std::string &id )
{
    string d;
    int size;

    // debug
    getline(s,d);
    cout<<"TRACE(loadsave.cpp): loadVector("<<id<<"): startLine: " << d << endl;
    // end debug

    loadString( s, d );
    if ( d != id )
    {
        cout<<"TRACE(loadsave.cpp): expected '" << id << "'" << endl;
        cout<<"TRACE(loadsave.cpp): found    '" << d  << "'" << endl;
        assert( d == id );
    }
        
    s >> size; getline(s,d);
    cout<<"TRACE(loadsave.cpp): loadVector: size = " << size << endl;
    v.resize( size );
    for ( unsigned int i=0; i < v.size(); i++ )
    {
        s >> v[i]; getline(s,d);
    }
    getline(s,d);
    cout<<"TRACE(loadsave.cpp): loadVector: endLine: " << d << endl;
    return s;
}

////////////////////////////////////////////////////////////////////////////////

void toFile(   QorcaModel &c, const std::string &fileName )
{
    std::ofstream f;
    f.open( fileName.c_str() );
    assert( f.is_open() );

    f << c;

    f.close();
}

void fromFile( QorcaModel &c, const std::string &fileName )
{
    std::ifstream f;
    f.open( fileName.c_str() );
    assert( f.is_open() );
    f >> c;

    f.close();
}

std::ostream &operator<<( std::ostream &s, const QorcaModel &model )
{
    cout<<"TRACE(loadsave.cpp): Saving Model: " << endl << toString(model) << endl;
    

    s << model.size() << std::endl;
    for ( unsigned int i=0; i < model.size(); i++ )
    {
        s << model[i]->def;
        s << model[i]->cfg;
        s << model[i]->geom;
    }
    return s;
}

std::istream &operator>>( std::istream &s, QorcaModel &model )
{
    cout<<"TRACE(loadsave.cpp): Loading model..." << endl;

    model.clear();
    string d;
    int size;
    s >> size; getline(s,d);

    cout<<"TRACE(loadsave.cpp): Model Size: " << size << endl;
    for ( int i=0; i < size; i++ )
    {
        cout<<"TRACE(loadsave.cpp): Loading component " << i << endl;


        ComponentDef  def;
        ComponentCfg  cfg;
        ComponentGeom geom;

        s >> def;
        s >> cfg;
        s >> geom;

        model.push_back( new ComponentModel( def, cfg, geom ) );

        cout<<"TRACE(loadsave.cpp): Loaded componentModel: " 
            << toString(*(model[model.size()-1])) << endl;

    }

    cout<<"TRACE(loadsave.cpp): Loaded model: " << endl << toString(model) << endl;

    return s;
}

std::ostream &operator<<( std::ostream &s, const orcadef::ComponentDef &d )
{
    saveString( s, d.tag );
    s << d.comments.size() << endl;
    for ( unsigned int i=0; i < d.comments.size(); i++ )
    {
        saveString( s, d.comments[i] );
    }
    saveVector<ProvidedDef>( s, d.provided, "ProvidedDef" );
    saveVector<RequiredDef>( s, d.required, "RequiredDef" );
    saveVector<ConfigDef>(   s, d.configs, "ConfigDef" );

    return s;
}

std::istream &operator>>( std::istream &s, orcadef::ComponentDef &d )
{
    string dummy;

    cout<<"TRACE(loadsave.cpp): Loading tag..." << endl;
    loadString( s, d.tag );
    cout<<"TRACE(loadsave.cpp): Got tag: " << d.tag << endl;
    cout<<"TRACE(loadsave.cpp): Getting numComments..." << endl;
    int numComments;
    s >> numComments; getline(s,dummy);

    cout<<"TRACE(loadsave.cpp): Loading " << numComments << " comments..." << endl;
    d.comments.resize( numComments );
    for ( int i=0; i < numComments; i++ )
    {
        loadString( s, d.comments[i] );
    }
    cout<<"TRACE(loadsave.cpp): Provided..." << endl;
    loadVector<ProvidedDef>( s, d.provided, "ProvidedDef" );
    cout<<"TRACE(loadsave.cpp): Required..." << endl;
    loadVector<RequiredDef>( s, d.required, "RequiredDef" );
    cout<<"TRACE(loadsave.cpp): Config..." << endl;
    loadVector<ConfigDef>(   s, d.configs, "ConfigDef" );
    return s;
}



std::ostream &operator<<( std::ostream &s, const ComponentGeom &g )
{
    s << g.id << " " << g.left << " " << g.top << " " << g.isHighlighted << endl;
    saveVector<InterfaceGeom>( s, g.provided, "InterfaceGeom" );
    saveVector<InterfaceGeom>( s, g.required, "InterfaceGeom" );
    return s;
}

std::istream &operator>>( std::istream &s, ComponentGeom &g )
{
    string dummy;
    int isH;
    s >> g.id >> g.left >> g.top >> isH;
    g.isHighlighted = isH != 0;
    getline(s,dummy);
    cout<<"TRACE(loadsave.cpp): id,left,top,isH:" << g.id << " " << g.left << " " << g.top << " " << g.isHighlighted << endl;

    loadVector<InterfaceGeom>( s, g.provided, "InterfaceGeom" );
    loadVector<InterfaceGeom>( s, g.required, "InterfaceGeom" );
    return s;
}

std::ostream &operator<<( std::ostream &s, const InterfaceGeom &g )
{
    s << g.x << " " << g.y << endl;
    return s;
}

std::istream &operator>>( std::istream &s, InterfaceGeom &g )
{
    s >> g.x >> g.y;
    string dummy;
    getline(s,dummy);
    return s;
}

std::ostream &operator<<( std::ostream &s, const orcadef::ProvidedDef &p )
{
    saveString( s, p.tag );
    saveString( s, p.type );
    saveString( s, p.name );
    return s;
}

std::istream &operator>>( std::istream &s, orcadef::ProvidedDef &p )
{
    loadString( s, p.tag );
    loadString( s, p.type );
    loadString( s, p.name );
    return s;
}

std::ostream &operator<<( std::ostream &s, const orcadef::RequiredDef &r )
{
    saveString( s, r.tag );
    saveString( s, r.type );
    s << r.proxy;
    return s;
}

std::istream &operator>>( std::istream &s, orcadef::RequiredDef &r )
{
    loadString( s, r.tag );
    loadString( s, r.type );
    s >> r.proxy;
    return s;
}

std::ostream &operator<<( std::ostream &s, const orcadef::ConfigDef &c )
{
    saveString( s, c.tag );
    saveString( s, c.type );
    saveString( s, c.value );
    return s;
}

std::istream &operator>>( std::istream &s, orcadef::ConfigDef &c )
{
    loadString( s, c.tag );
    loadString( s, c.type );
    loadString( s, c.value );
    return s;
}

std::ostream &operator<<( std::ostream &s, const orca::FQInterfaceName &i )
{
    saveString( s, i.platform );
    saveString( s, i.component );
    saveString( s, i.iface );
    return s;
}

std::istream &operator>>( std::istream &s, orca::FQInterfaceName &i )
{
    loadString( s, i.platform );
    loadString( s, i.component );
    loadString( s, i.iface );
    return s;
}


std::ostream &operator<<( std::ostream &s, const orcadef::ComponentCfg &c )
{
    s << c.fqname;
    s << c.fqexecutable;
    saveString( s, c.endpoints );
    saveVector<ProvidedCfg>( s, c.provided, "ProvidedCfg" );
    saveVector<RequiredCfg>( s, c.required, "RequiredCfg" );
    saveVector<ConfigCfg>(   s, c.configs, "ConfigCfg" );

    return s;
}

std::istream &operator>>( std::istream &s, orcadef::ComponentCfg &c )
{
    cout<<"TRACE(loadsave.cpp): loading ComponentCfg:" << endl;

    s >> c.fqname;
    s >> c.fqexecutable;
    
    loadString( s, c.endpoints );

    cout<<"TRACE(loadsave.cpp): name: " << orcaice::toString(c.fqname) << endl;
    cout<<"TRACE(loadsave.cpp): exe:  " << orcaice::toString(c.fqexecutable) << endl;
    cout<<"TRACE(loadsave.cpp): ends: " << c.endpoints << endl;


    cout<<"TRACE(loadsave.cpp): Loading provided..." << endl;
    loadVector<ProvidedCfg>( s, c.provided, "ProvidedCfg" );
    cout<<"TRACE(loadsave.cpp): Loading required..." << endl;
    loadVector<RequiredCfg>( s, c.required, "RequiredCfg" );
    cout<<"TRACE(loadsave.cpp): Loading config..." << endl;
    loadVector<ConfigCfg>(   s, c.configs, "ConfigCfg" );
    return s;
}


std::ostream &operator<<( std::ostream &s, const orcadef::ConfigCfg &c )
{
    saveString( s, c.tag );
    saveString( s, c.value );
    return s;
}

std::istream &operator>>( std::istream &s, orcadef::ConfigCfg &c )
{
    loadString( s, c.tag );
    loadString( s, c.value );
    return s;
}


std::ostream &operator<<( std::ostream &s, const orcadef::RequiredCfg &r )
{
    saveString( s, r.tag );
    s << r.proxy;
    return s;
}

std::istream &operator>>( std::istream &s, orcadef::RequiredCfg &r )
{
    loadString( s, r.tag );
    s >> r.proxy;
    return s;
}


std::ostream &operator<<( std::ostream &s, const orcadef::ProvidedCfg &c )
{
    saveString( s, c.tag );
    saveString( s, c.name );
    return s;
}

std::istream &operator>>( std::istream &s, orcadef::ProvidedCfg &c )
{
    loadString( s, c.tag );
    loadString( s, c.name );
    return s;
}

std::ostream &operator<<( std::ostream &s, const orca::FQExecutableName &e )
{
    saveString( s, e.executable );
    saveString( s, e.host );
    return s;
}

std::istream &operator>>( std::istream &s, orca::FQExecutableName &e )
{
    loadString( s, e.executable );
    loadString( s, e.host );
    return s;
}

std::ostream &operator<<( std::ostream &s, const orca::FQComponentName &c )
{
    saveString( s, c.platform );
    saveString( s, c.component );
    return s;
}

std::istream &operator>>( std::istream &s, orca::FQComponentName &c )
{
    loadString( s, c.platform );
    loadString( s, c.component );
    return s;
}


}
