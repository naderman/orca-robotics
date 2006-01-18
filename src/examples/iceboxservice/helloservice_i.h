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
// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef HELLO_SERVICE_I_H
#define HELLO_SERVICE_I_H

#include <IceBox/IceBox.h>

#ifndef HELLO_API
#   define HELLO_API ICE_DECLSPEC_EXPORT
#endif

class HELLO_API HelloServiceI : public IceBox::Service
{
public:

    HelloServiceI();
    virtual ~HelloServiceI();

    virtual void start(const ::std::string&,
               const Ice::CommunicatorPtr&,
               const Ice::StringSeq&);

    virtual void stop();

private:

    Ice::ObjectAdapterPtr adapter_;
};

#endif
