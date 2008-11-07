/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBE_FACTORY_H
#define ORCA_ORCAPROBE_FACTORY_H

#include <vector>
#include <string>
#include <orca/common.h>
#include <orca/ocm.h>

namespace orcaice
{
    class Context;
}

namespace orcaprobe
{

class InterfaceProbe;
class AbstractDisplay;

//!
//! Factory of interface probes
//!
//! Intended for use as a base-class by logfactory and replayfactory
//!
class Factory
{
public:
    virtual ~Factory() {};

    //! Returns a vector of all supported interface types.
    //! Use @p addSupportedType to define a list of supported types during
    //! initialization.
    std::vector<std::string> supportedTypes() const;

    //! Returns TRUE if the specified interface type is supported by this factory.
    //! Use @p addSupportedType to define a list of supported types during
    //! initialization.
    bool isSupported( const std::string &interfaceType ) const;

    //! Creates a probe of specified type. Returns a pointer to it if
    //! successful or a NULL pointer if it fails.
    virtual InterfaceProbe* create( const std::string           & id, 
                                    const orca::FQInterfaceName & name, 
                                    const Ice::ObjectPrx        & adminPrx,
                                    AbstractDisplay             & display, 
                                    const orcaice::Context      & context ) = 0;
protected:

    //! Adds type to the list of supported interfaces.
    void addSupportedType( const std::string & interfaceType );

private:
    std::vector<std::string> types_;
};

} // namespace


// Function for instantiating plug-in factories.
// A plug-in library must have a function like so:
// extern "C" {
//     orcalog::Factory *createFactory();
// }
typedef orcaprobe::Factory *FactoryMakerFunc();


#endif
