/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_ICEGRID_UTILITIES_H
#define ORCAICE_ICEGRID_UTILITIES_H

#include <IceGrid/Query.h>

#include <orcaice/context.h>
#include <orcaice/configutils.h>
#include <orcaice/exceptions.h>
// #include <orcaice/stringutils.h>  // just for debugging
#include <orcaice/printutils.h>

namespace orcaice
{
/*!
 *  @name IceGrid-Related Functions
 */
//@{

/*!
Tries to ping the default Locator and, based on success or the exceptions
it catches, determines if the registry is reachable.
 */
bool isRegistryReachable( const Context& context );

//! Returns a proxy to the default Query object, based on the default Locator of
//! communicator. Returns null proxy on failure.
IceGrid::QueryPrx getDefaultQuery( const orcaice::Context& context );

//! Returns identity of the component's Admin object.
Ice::Identity toAdminIdentity( const orca::FQComponentName& );

//! Parses category of the identity to extract a fully qualified component name.
//! This is the inverse of toIdentityCategory();
//! Expects an input string in the form of @c platform.component
//! A null structure is returned if no delimeter is present.
//! Does not handle the case of multiple delimeters.
orca::FQComponentName toComponentName( const Ice::Identity& adminId );

//! Parses home proxy to a fully qualified name of the component to whome it belongs.
orca::FQComponentName toComponentName( const Ice::ObjectPrx& homePrx );

/*!
Contacts the default Locator interface of the Registry and looks up component admin proxy.

Before creating the interface, the fully-qualified component name is checked and platform 'local' is replaced 
with current hostname.
*/
Ice::ObjectPrx getComponentAdmin( const orcaice::Context& context, const orca::FQComponentName& fqName );

/*!
Contacts the default Query interface of the Registry and looks up all registered admin proxies.
*/
Ice::ObjectProxySeq getAllComponentAdmins( const orcaice::Context& context );

/*!
Orca convention for naming admin interface facets. The reason for encoding the platform and component name in
the facet name: to allow sharing of Communicator inside the IceBox by multiple components (all of them will try
to add their admin facets to the same Communicator).

The following admin interfaces are currently supported
- ::Ice::Process
- ::Ice::PropertiesAdmin
- ::orca::Home
- ::orca::Status
- ::orca::Tracer

The fully-qualified component name is checked and platform 'local' is replaced with current hostname.

Throws bxutilacfr::Exception when called with unsupported object type.
*/
std::string toAdminFacet( const orca::FQComponentName& fqName, const std::string& interfaceType );

/*!
Tries to ice_ping() the remote Admin interface specified with a fully-qualified component name and 
the interface facet name. Returns TRUE if the ping was successful and FALSE if not.
Writes diagnostic information into @c diagnostic string.
 
Before making any connections, the fully-qualified component name is checked and platform 'local' is replaced 
with current hostname.

Not all objects can be added to the admin interface, see toAdminFacet() for the list of supported interfaces.

Catches all exceptions. Does not throw.
 */
bool isAdminInterfaceReachable( const Context& context, const orca::FQComponentName& fqName, const std::string& interfaceType,
                           std::string& diagnostic );

/*!
Adds the @p object as a facet to the component's Admin interface.

Not all objects can be added to the admin interface, see toAdminFacet() for the list of supported interfaces.

Before creating the interface, the fully-qualified component name is checked and platform 'local' is replaced 
with current hostname.

This funciton may throw Ice::AlreadyRegisteredException. We don't catch it internally
because this is a specialized function which only used in internal code.

If successful, registers the interface with Home under the name which is the same as the Admin facet.

@verbatim
Ice::ObjectPtr obj = new MyObjectI;
orcaice::createAdminInterface( context_, obj, context_.name() );
@endverbatim
 */
void createAdminInterface( const Context& context, Ice::ObjectPtr& object, const orca::FQComponentName& fqname );

/*!
Special connect function to be used with interfaces hosted as facets of component's Admin interface.

Not all objects can be added to the admin interface, see toAdminFacet() for the list of supported interfaces.

Before making any connections, the fully-qualified component name is checked and platform 'local' is replaced 
with current hostname.

In the event of a failed connection, adds an interpretive message and re-throws a NetworkException.

Example 1. Connect to your own Status.
@verbatim
orca::StatusPrx statusPrx;
orcaice::connectToAdminInterface<orca::Status,orca::StatusPrx>( context_, statusPrx, context_.name() );
@endverbatim
@note Registration of Admin interface with the registry happens in a separate thread. If you really want to connect
to one of your own Admin interfaces you have to sleep for a bit or try several times.

Example 2. Connect to a remote Process.
@verbatim
Ice::ProcessPrx processPrx;
orca::FQComponentName fqName;
fqName.platform = "papa";
fqName.component = "murphy";
orcaice::connectToAdminInterface<Ice::Process,Ice::ProcessPrx>( context_, processPrx, fqName );
@endverbatim
*/
// template<class InterfaceProxyType>
// void connectToAdminInterfaceWithFacet( const Context& context, InterfaceProxyType interfacePrx, const std::string& facet, 
template<class InterfaceType, class InterfaceProxyType>
void connectToAdminInterface( const Context& context, InterfaceProxyType& interfacePrx, const orca::FQComponentName& fqname )
{   
    orca::FQComponentName resolvedFqname = orcaice::resolveLocalPlatform( context, fqname );
    
// std::cout<<"DEBUG: "<<__func__<<"(): will look for Admin interface of ="<<orcaice::toString(resolvedFqname)<<std::endl;

    const std::string interfaceType = InterfaceType::ice_staticId();
    const std::string facetName = orcaice::toAdminFacet( resolvedFqname, interfaceType );
        
// std::cout<<"DEBUG: "<<__func__<<"(): will connect to facet="<<facetName<<std::endl;

    //
    // This is the generic proxy to the Admin object. Cannot be used as such because it does not have a default facet.
    //
    Ice::ObjectPrx objectPrx = orcaice::getComponentAdmin( context, resolvedFqname );

    if ( !objectPrx )
        throw gbxutilacfr::Exception( ERROR_INFO, "Registry returned null admin proxy." );

// std::cout<<"DEBUG: "<<__func__<<"(): got admin proxy ="<<objectPrx->ice_toString()<<std::endl;

    //
    // Change proxy to the user-supplied facet name.
    //
    objectPrx = objectPrx->ice_facet( facetName );

    // check with the server that the one we found is of the right type
    // the check operation is remote and may fail (see sec.6.11.2)
    try {
        interfacePrx = InterfaceProxyType::checkedCast( objectPrx );
        // got some answer, check that it's the right type
        if ( !interfacePrx ) {
            // contains another remote call ice_id() !!!
            std::string errString = "Admin interface with facet '" + facetName + "' is of wrong type."+
                "  Tried to connect proxy of type "+InterfaceType::ice_staticId()+
                " to remote interface of type "+objectPrx->ice_id();
            orcaice::initTracerWarning( context, errString, 2 );
            throw orcaice::TypeMismatchException( ERROR_INFO, errString );
        }
    }
    catch ( const Ice::ConnectionRefusedException& e )
    {
        std::stringstream ss;
        ss << "(while connecting to Admin interface with facet '" << facetName << "') cannot reach the adaptor: "<<e.what();
        orcaice::initTracerWarning( context, ss.str(), 2 );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }
    catch ( const Ice::ObjectNotExistException& e )
    {
        std::stringstream ss;
        ss << "(while connecting to Admin interface with facet '" << facetName << "') reached the adaptor but not the interface: "<<e.what();
        orcaice::initTracerWarning( context, ss.str(), 2 );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }
    catch ( const Ice::FacetNotExistException& e )
    {
        std::stringstream ss;
        ss << "(while connecting to Admin interface with facet '" << facetName << "') reached the adaptor and the interface but not the facet: "<<e.what();
        orcaice::initTracerWarning( context, ss.str(), 2 );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }
    catch ( const std::exception& e )
    {
        std::stringstream ss;
        ss << "(while connecting to Admin interface with facet '" << facetName << "') something unexpected: "<<e.what();
        orcaice::initTracerWarning( context, ss.str(), 2 );
        throw orcaice::NetworkException( ERROR_INFO, ss.str() );
    }
}

//@}

} // namespace

#endif
