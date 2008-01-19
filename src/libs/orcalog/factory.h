/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCALOG_FACTORY_H
#define ORCA_ORCALOG_FACTORY_H

#include <vector>
#include <string>

namespace orcalog
{

//!
//! Factory of slave loggers/logplayers.
//!
//! Intended for use as a base-class by logfactory and replayfactory
//!
class Factory
{
public:
    //! Returns a vector of all supported interface types.
    //! Use addSupportedType() to define a list of supported types during
    //! initialization.
    std::vector<std::string> supportedTypes() const;

    //! Returns TRUE if the specified interface type is supported by this factory.
    //! Use addSupportedType() to define a list of supported types during
    //! initialization.
    bool isSupported( const std::string &interfaceType ) const;

protected:

    //! Adds type to the list of supported interfaces.
    void addSupportedType( const std::string & interfaceType );

private:
    std::vector<std::string> types_;
};

}


#endif
