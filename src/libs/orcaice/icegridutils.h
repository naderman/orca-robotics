/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_ICEGRID_UTILITIES_H
#define ORCAICE_ICEGRID_UTILITIES_H

#include <Ice/Ice.h>
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

Ice::ObjectPrx getComponentAdmin( const orcaice::Context& context, const orca::FQComponentName& component );

Ice::ObjectProxySeq getAllComponentAdmins( const orcaice::Context& context );

/*!
Tries to ice_ping() the remote Admin interface specified with a fully-qualified component name and 
the interface facet name. Returns TRUE if the ping was successful and FALSE if not.
Writes diagnostic information into @c diagnostic string.
 
Before making any connections, the fully-qualified component name is checked and platform 'local' is replaced 
with current hostname.

Catches all exceptions. Does not throw.  
  
Implementation note: this function does not need to be templated because
ice_ping() is implemented by all Ice objects regardless of type.
 */
bool isAdminInterfaceReachable( const Context& context, const orca::FQComponentName& fqname, const std::string& facetName,
                           std::string& diagnostic );

/*!
Special connect function to be used with interfaces hosted as facets of component's Admin interface.

The following admin interfaces are currently supported with corresponding facet names (capitalization in 
facet name is important!):
- ::Ice::Process
- ::Ice::PropertiesAdminPrx
- ::orca::Home
- ::orca::Status
- ::orca::Tracer
If an unsupported facet is requested, gbxutilacfr::Exception is thrown.

In the event of a failed connection, adds an interpretive message and re-throws a NetworkException.

Before making any connections, the fully-qualified component name is checked and platform 'local' is replaced 
with current hostname.

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
//     if ( facet != "Process" && facet != "Properties" && facet != "Home" && facet != "Status" && facet != "Tracer" )
//         throw gbxutilacfr::Exception( ERROR_INFO, std::string("Unsupported Admin facet: ")+facet );

    std::string interfaceType = InterfaceType::ice_staticId();
    std::string facetName;

    if ( interfaceType == "::Ice::Process" )
        facetName = "Process";
    else if ( interfaceType == "::Ice::PropertiesAdmin" )
        facetName = "Properties";
    else if ( interfaceType == "::orca::Home" )
        facetName = "Home";
    else if ( interfaceType == "::orca::Status" )
        facetName = "Status";
    else if ( interfaceType == "::orca::Tracer" )
        facetName = "Tracer";
    else
        throw gbxutilacfr::Exception( ERROR_INFO, std::string("Orca does not support Admin interface type: ")+interfaceType );
        
// std::cout<<"DEBUG: "<<__func__<<"(): will connect to facet="<<facetName<<std::endl;

    orca::FQComponentName resolvedFqname = orcaice::resolveLocalPlatform( context, fqname );
    
// std::cout<<"DEBUG: "<<__func__<<"(): will look for Admin interface of ="<<orcaice::toString(resolvedFqname)<<std::endl;

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
