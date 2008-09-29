/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA_PATH_UTILS_H
#define ORCA_PATH_UTILS_H

#include <hydroqguipath/pathutils.h>
#include <orca/pathfollower2d.h>
#include <orcaice/orcaice.h>

namespace orcaqgui2d {

//
// Conversion functions
//
void guiPathToOrcaPath( const hydroqguipath::GuiPath &in, 
                        orca::Path2d                 &out, 
                        int                           numLoops = 1, 
                        float                         timeOffset = 0.0 );

void orcaPathToGuiPath( const orca::Path2d     &in, 
                        hydroqguipath::GuiPath &out );

//
// Reading stuff from cfg file
//
hydroqguipath::WaypointSettings readWaypointSettings( const Ice::PropertiesPtr &props, 
                                                      const std::string        &tag );

bool readActivateImmediately( const Ice::PropertiesPtr &props, 
                              const std::string        &tag );

QString readDumpPath( const Ice::PropertiesPtr &props, 
                      const std::string        &tag );


}

#endif
