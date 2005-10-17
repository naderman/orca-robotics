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

#ifndef ORCA2_LASER_INTERFACE_ICE
#define ORCA2_LASER_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{

// Define messages first
sequence<float>        RangeSequence;
sequence<byte>         IntensitySequence;

class LaserConfig extends OrcaObject
{
    float rangeResolution;
    float angleIncrement;
};

class LaserGeometry extends OrcaObject
{
    // Offset of the centre of the laser from the robot, int the robot CS
    Frame3d offset;

    // Dimensions of the laser
    Size3d  size;
};

class LaserData extends OrcaObject
{
    RangeSequence     ranges;
    IntensitySequence intensities;
    float             startAngle;
    float             angleIncrement;
};

interface Laser
{
    // Functions for reading from the laser
    nonmutating LaserData      getData();
    nonmutating LaserConfig    getConfig();
    nonmutating LaserGeometry  getGeometry();

    idempotent  void setConfig( LaserConfig config );
};

interface LaserConsumer
{
    void consumeData( LaserData obj );
};

}; // module

#endif
