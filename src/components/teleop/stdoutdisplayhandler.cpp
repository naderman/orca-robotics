/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "stdoutdisplayhandler.h"

using namespace std;
using namespace teleop;

void 
StdoutDisplayHandler::displayEvent( const Event e )
{
    switch ( e )
    {
    case SentNewCommand :
    {
        cout<<"C"<<flush;
        break;
    }
    case SentRepeatCommand :
    {
        cout<<"."<<flush;
        break;
    }
    default :
    {
        cout<<"x"<<flush;
    }
    }
}

void 
StdoutDisplayHandler::displayCommand( const orca::Velocity2dCommand& command,
                                 const bool vx, const bool vy, const bool w )
{
    cout<<"\n"<<orcaice::toString( command );
    
    if ( vx ) cout<<"Vx LIMITED. ";
    if ( vy ) cout<<"Vy LIMITED. ";
    if ( w ) cout<<"W LIMITED. ";

    cout<<endl;
}
