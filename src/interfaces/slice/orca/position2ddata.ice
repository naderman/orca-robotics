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

#ifndef ORCA2_POSISTION2D_DATA_ICE
#define ORCA2_POSISTION2D_DATA_ICE

#include <orca/orca.ice>

module orca
{

//! using Orca variable names but lump Pose2D and RobotVelocity2D objects together -- Player style.
class Position2dData extends OrcaObject
{
    double x;
    double y;
    double heading;
    double speed;
    double sidespeed;
    double turnrate;
    bool stalled;
};

//! using Orca variable names but lump Pose2D and RobotVelocity2D objects together -- Player style.
struct SimplePosition2dData
{
    Time timeStamp;
    double x;
    double y;
    double heading;
    double speed;
    double sidespeed;
    double turnrate;
    bool stalled;
};

}; // module

#endif
