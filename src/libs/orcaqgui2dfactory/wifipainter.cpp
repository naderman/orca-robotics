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
#include <cmath> 
#include <qpainter.h>

#include "wifipainter.h"

using namespace std;

namespace orcaqgui2d
{

WifiPainter::WifiPainter()
{
}

WifiPainter::~WifiPainter()
{
}

void
WifiPainter::clear()
{
}

void 
WifiPainter::setData( const orca::WifiData& wifiData )
{
}

void WifiPainter::paint( QPainter *painter, int z )
{
    if ( z != hydroqguielementutil::Z_FOREGROUND ) return;
}

} // namespace

