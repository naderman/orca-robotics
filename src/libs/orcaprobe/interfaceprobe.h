/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBE_INTERFACE_PROBE_H
#define ORCA_ORCAPROBE_INTERFACE_PROBE_H

#include <orca/common.h>
#include <orcacm/types.h>
#include <orcaice/context.h>

namespace orcaprobe
{

class AbstractDisplay;

enum OperationIndex
{
    IcePingIndex=0,
    UserIndex
};

class InterfaceProbe : public IceUtil::Shared
{

public:
    //! Constructor
    InterfaceProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, 
                AbstractDisplay& display, const orcaice::Context& context );
        
    virtual ~InterfaceProbe();

    //! Returns a listing of operations supported by this probe.
    std::vector<orcacm::OperationHeader> operations() { return operations_; };

    //! Loads and executes the operation. The index matches the listing returned by
    //! @p operations(). Note that the first several operations are generic to all
    //! interfaces.
    //! Returns 0 on success good, 1 on error.
    int loadOperation( const int index, orcacm::OperationData& data );

protected:

    //! Fully-qualified name of the interface.
    orca::FQInterfaceName name_;

    //! Interface type, e.g. ::orca::LaserScanner2d. It is set by the derived class.
    std::string id_;

    //! keep a direct link to display so if get some data asynchronously from browser
    //! (e.g. through subscription) we can display it. it's safe because all of display's
    //! public API is thread-safe.
    AbstractDisplay& display_;

    //! Component communication context with pointers to Communicator, Tracer, etc.
    orcaice::Context ctx_;

    //! Proxy to the remote object. This probe will use it to call the remote operations.
    Ice::ObjectPrx prx_;

    //! Adds @p name and @p signature of an interface operation to the listing
    //! which will be later returned by @p operations(). The operation may or may not be supported
    //! by the interface probe.
    void addOperation( const std::string& name, bool isSupported, const std::string& signature="" );

    //! Convenience function which assumes that the operation is supported.
    void addOperation( const std::string& name, const std::string& signature="", bool isSupported=true );

    //! Implement this function in the derived probe class.
    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );

    //! fills out the header information, does not touch the 'result' field
    void fillOperationData( const int index, orcacm::OperationData& data );

private:
    std::vector<orcacm::OperationHeader> operations_;

    // Pings the remote interface.
    int loadIcePing( orcacm::OperationData& data );

};
//! Ice smart pointer to EventLoop
typedef IceUtil::Handle<InterfaceProbe> InterfaceProbePtr;

} // namespace

#endif
