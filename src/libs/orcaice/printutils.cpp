/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "printutils.h"

#include "orcaice.h"

using namespace std;

namespace orcaice
{

void initTracerInfo( const std::string & message )
{
    cout<<"orca: "<<message<<endl;
}

void initTracerWarning( const std::string & message )
{
    cout<<"orca: warning: "<<message<<endl;
}

void initTracerError( const std::string & message )
{
    cerr<<"orca: error: "<<message<<endl;
}


void initTracerInfo( const orcaice::Context & context, const std::string & message, const int level, bool localOnly )
{
    if ( &context.tracer() )  {
        context.tracer().info( message, level, localOnly );
    }
    else {
        // the tracer is not initialized yet
        cout<<"orca: "<<message<<endl;
    }  
}

void initTracerWarning( const orcaice::Context & context, const std::string & message, const int level, bool localOnly )
{
    if ( &context.tracer() )  {
        context.tracer().warning( message, level, localOnly );
    }
    else {
        // the tracer is not initialized yet
        cout<<"warning: "<<message<<endl;
    }
}

void initTracerError( const orcaice::Context & context, const std::string & message, const int level, bool localOnly )
{
    if ( &context.tracer() )  {
        context.tracer().error( message, level, localOnly );
    }
    else {
        // the tracer is not initialized yet
        cout<<"orca: error: "<<message<<endl;
    }
}

} // namespace
