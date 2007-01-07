/*
 * Orca Project: Components for robotics 
             http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_HOME_H
#define ORCAICE_HOME_H

namespace orcaice
{

/*!
@brief Local API to component information.
 *
 */
// implem notes:
//   - The local API of this class could also be defined as a local interface in Slice.
class Home
{
public:
    virtual ~Home() {}; 
    
    //! Adds provided interface to dynamic component information
    virtual void addProvidedInterface( const orca::ProvidedInterface& iface ) = 0;

    //! Adds required interface to dynamic component information
    virtual void addRequiredInterface( const orca::RequiredInterface& iface ) = 0;

    //! Removes provided interface from dynamic component information
    virtual void removeProvidedInterface( const std::string& name ) = 0;

    //! Removes required interface from dynamic component information
    virtual void removeRequiredInterface( const std::string& name ) = 0;
};

} // namespace

#endif
