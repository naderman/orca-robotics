/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ICEGRIDMON_OBSERVER_I_H
#define ORCA2_ICEGRIDMON_OBSERVER_I_H

#include <IceGrid/Observer.h>
#include <orcaice/context.h>

namespace icegridmon
{

class NodeObserverI : virtual public IceGrid::NodeObserver
{
public:
    NodeObserverI( const orcaice::Context & context );

    virtual ~NodeObserverI() {};

    virtual void init(const ::IceGrid::NodeDynamicInfoSeq&,
		      const Ice::Current&);

    virtual void nodeUp(const ::IceGrid::NodeDynamicInfo&,
			const Ice::Current&);

    virtual void nodeDown(const ::std::string&,
			  const Ice::Current&);

    virtual void updateServer(const ::std::string&,
			      const ::IceGrid::ServerDynamicInfo&,
			      const Ice::Current&);

    virtual void updateAdapter(const ::std::string&,
			       const ::IceGrid::AdapterDynamicInfo&,
			       const Ice::Current&);
private:
    orcaice::Context context_;

};

class RegistryObserverI : virtual public IceGrid::RegistryObserver
{
public:
    RegistryObserverI( const orcaice::Context & context );

    virtual ~RegistryObserverI() {};

    virtual void init(::Ice::Int,
		      const ::IceGrid::ApplicationDescriptorSeq&,
		      const ::IceGrid::AdapterInfoSeq&,
		      const ::IceGrid::ObjectInfoSeq&,
		      const Ice::Current&);

    virtual void applicationAdded(::Ice::Int,
				  const ::IceGrid::ApplicationDescriptor&,
				  const Ice::Current&);

    virtual void applicationRemoved(::Ice::Int,
				    const ::std::string&,
				    const Ice::Current&);

    virtual void applicationUpdated(::Ice::Int,
				    const ::IceGrid::ApplicationUpdateDescriptor&,
				    const Ice::Current&);

    virtual void adapterAdded(::Ice::Int,
			      const ::IceGrid::AdapterInfo&,
			      const Ice::Current&);

    virtual void adapterUpdated(::Ice::Int,
				const ::IceGrid::AdapterInfo&,
				const Ice::Current&);

    virtual void adapterRemoved(::Ice::Int,
				const ::std::string&,
				const Ice::Current&);

    virtual void objectAdded(::Ice::Int,
			     const ::IceGrid::ObjectInfo&,
			     const Ice::Current&);

    virtual void objectUpdated(::Ice::Int,
			       const ::IceGrid::ObjectInfo&,
			       const Ice::Current&);

    virtual void objectRemoved(::Ice::Int,
			       const ::Ice::Identity&,
			       const Ice::Current&);

private:
    orcaice::Context context_;
};

} // namespace

#endif
