/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAIFACEIMPL_BUTTON_IMPL_H
#define ORCA2_ORCAIFACEIMPL_BUTTON_IMPL_H

#include <orca/button.h>
#include <IceStorm/IceStorm.h>

#include <hydroutil/proxy.h>
#include <hydroutil/notify.h>
#include <orcaice/context.h>

namespace hydroutil {
    class Thread;
}

namespace orcaifaceimpl {

//!
//! Implements the Button interface: Handles remote calls.
//!
class ButtonImpl : public IceUtil::Shared,
                   public hydroutil::Notify<bool> // Note: the 'bool' here is meaningless.
{
friend class ButtonI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    ButtonImpl( const std::string& interfaceTag,
                const orcaice::Context& context );
    //! constructor using interfaceName
    ButtonImpl( const orcaice::Context& context,
                const std::string& interfaceName );
    ~ButtonImpl();

    //! Sets up interface and connects to IceStorm. May throw hydroutil::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( hydroutil::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

private:

    // remote call implementations, mimic (but do not inherit) the orca interface
    void internalPress();

    const std::string                 interfaceName_;

    orcaice::Context                  context_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;
};
typedef IceUtil::Handle<ButtonImpl> ButtonImplPtr;

} // namespace

#endif
