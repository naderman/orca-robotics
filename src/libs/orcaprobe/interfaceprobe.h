/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAPROBE_INTERFACE_PROBE_H
#define ORCA2_ORCAPROBE_INTERFACE_PROBE_H

#include <orca/orca.h>
#include <orcacm/types.h>
#include <orcaice/context.h>

namespace orcaprobe
{

class DisplayDriver;

class InterfaceProbe
{

public:
    InterfaceProbe( const orca::FQInterfaceName & name, DisplayDriver & display,
                                const orcaice::Context & context );
        
    virtual ~InterfaceProbe() {};

    //! Returns a listing of operations supported by this probe.
    std::vector<orcacm::OperationHeader> operations() { return operations_; };

    //! Loads and executes the operation. The index matches the listing returned by
    //! @p operations(). Returns 0 on success good, 1 on error.
    virtual int loadOperation( const int index, orcacm::OperationData & data )=0;

protected:

    orca::FQInterfaceName name_;
    std::string id_;
    std::vector<orcacm::OperationHeader> operations_;

    void addOperation( const std::string & name, const std::string & signature="" );
    
    // keep a direct link to display so if get some data asynchronously from browser
    // (e.g. through subscription) we can display it. it's safe because all of display's
    // public API is thread-safe.
    DisplayDriver & display_;

    orcaice::Context context_;
    Ice::ObjectPrx prx_;

    // fills out the header information, does not touch the 'result' field
    void fillOperationData( const int index, orcacm::OperationData & data );
};

} // namespace

#endif
