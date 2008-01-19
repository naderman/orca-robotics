/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCALOG_DUMMY_REPLAYER_H
#define ORCA_ORCALOG_DUMMY_REPLAYER_H

#include <orcalog/replayer.h>

namespace orcalog
{

//! This one can be used for unsupported or disabled interfaces.
class DummyReplayer : public Replayer
{
public:
    DummyReplayer() {}

    void init () {}

    void replayData( int index ) {}

    std::string toString() const { return "dummy"; }
};

} // namespace

#endif
