/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROBE_BROWSER_DRIVER_INTERFACE_H
#define ORCA2_PROBE_BROWSER_DRIVER_INTERFACE_H

namespace probe
{

enum BrowserEvent
{
    ActivateEvent,
    ReloadEvent,
    UpEvent,
    FaultEvent,
    DeactivateEvent,
    FilterEvent,
    TopEvent,
    // this must be the last event (with the highest number)
    // because actual picks are added to this one.
    PickEvent=1000
};

} // namespace

#endif
