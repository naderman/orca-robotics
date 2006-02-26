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

#include <iostream>
#include <orcaice/orcaice.h>

#include "stdoutdisplayhandler.h"

using namespace std;
using orcaice::operator<<;

void StdoutDisplayHandler::displayEvent( const Event e )
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

void StdoutDisplayHandler::displayCommand( const orca::Velocity2dCommandPtr & command,
                                 const bool vx, const bool vy, const bool w )
{
    cout<<"\n"<<command;
    
    if ( vx ) cout<<"Vx LIMITED. ";
    if ( vy ) cout<<"Vy LIMITED. ";
    if ( w ) cout<<"W LIMITED. ";

    cout<<endl;
}
