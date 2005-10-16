/*
 *  Orca-Components: Components for robotics.
 *  
 *  Copyright (C) 2004
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>

#include "teleopcontext.h"
#include "outputdriver.h"
#include "keyboarddriver.h"

using namespace std;

TeleopContext::TeleopContext()
{
}

TeleopContext::~TeleopContext()
{
}

void TeleopContext::startup()
{
    //
    // NETWORK
    //
    netDriver_ = new OutputDriver( &commandProxy );
    // dodgy hack: non-standard function
    netDriver_->setupComms( comm_ );
    netDriver_->activate();

    //
    // HARDWARE
    //
    hwDriver_ = new KeyboardDriver( &commandProxy );
    hwDriver_->setup( comm_->getProperties() );

    hwDriver_->activate();
}

void TeleopContext::shutdown()
{
    netDriver_->deactivate();
    cout<<"joining network hwDriver_..."<<endl;
    netDriver_->getThreadControl().join();

    hwDriver_->deactivate();
    cout<<"joining input hwDriver_... Hit any key please."<<endl;
    hwDriver_->getThreadControl().join();
}

void TeleopContext::FSMError(const char* t, const char* s)
{
    std::cerr << "Transition error: " << t << " in state " << s << std::endl;
}
