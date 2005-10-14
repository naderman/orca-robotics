/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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

// include defnition of Ice runtime
#include <Ice/Ice.h>

#include "rmpcomponent.h"

using namespace std;

class App : virtual public Ice::Application
{
    public:
        virtual int run(int, char * []);
};

int App::run( int argc, char* argv[] )
{
    // create our component and start it (it will not return)
    RmpComponent comp;
    comp.start( "SegwayRmpUsb", communicator(), Ice::StringSeq() );

    //communicator()->waitForShutdown();

    return 0;
}

int main(int argc, char * argv[])
{
    App app;
    return app.main(argc, argv);
}
