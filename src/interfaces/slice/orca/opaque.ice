/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_OPAQUE_ICE
#define ORCA_OPAQUE_ICE

#include <orca/orca.ice>

module orca
{

class Opaque extends OrcaObject
{
    ByteSequence payload;
};

};

#endif
