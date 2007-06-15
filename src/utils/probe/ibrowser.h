/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROBE_BROWSER_INTERFACE_H
#define ORCA2_PROBE_BROWSER_INTERFACE_H

namespace probe
{

class IBrowser
{

public:

    virtual ~IBrowser() {};

    virtual void chooseActivate()=0;

    virtual void chooseReload()=0;

    virtual void chooseUp()=0;

    virtual void chooseTop()=0;

    virtual void choosePick( int )=0;

    virtual void chooseFilter( const std::string & )=0;

    virtual void chooseDeactivate()=0;

};

} // namespace

#endif
