/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <string>

#include <QApplication>     // for qApp

#include <orcaobj/orcaobj.h> // just for toString()

#include "modelthread.h"
#include "homeevent.h"

using namespace std;

namespace orcaqcm
{

ModelThread::ModelThread( orcaqcm::NetworkThread & networkThread )
{
    // set the handler right away
    networkThread.homePipe_.setNotifyHandler( this );
}

ModelThread::~ModelThread()
{
}

// we'll post an event which will handled by us in customEvent() inside the GUI thread
void
ModelThread::handleData( const orcacm::ComponentData & data )
{
    //cout<<"ModelThread::handleData: handling home message"<<endl;
    
    qApp->postEvent( this, (QEvent*)new HomeEvent( data ) );
}

// handling an event posted by us from handleData()
void
ModelThread::customEvent( QEvent* e )
{
    HomeEvent* he = dynamic_cast<HomeEvent*>(e);
    assert( he && "event should be a HomeEvent" );

    // make sure the component is reachable
    if ( !he->data_.isReachable ) {
        // unreachable, will also set all children as disconnected
        model_.setComponent(
                QString::fromStdString( he->data_.locatorString ),
                QString::fromStdString( he->data_.adminAddress ),
                QString::fromStdString( he->data_.name.platform ),
                QString::fromStdString( he->data_.name.component ),
                "",
                false,
                he->data_.timeUp );
    }
    else {
        // component is reachable
        //cout<<"reg proxy: "<<he->registryAddress.toStdString()<<endl;

        // provided interfaces
        for ( unsigned int j=0; j<he->data_.provides.size(); ++j ) {
            bool isProvided = true;
            model_.setInterface(
                QString::fromStdString( he->data_.locatorString ),
                QString::fromStdString( he->data_.adminAddress ),
                QString::fromStdString( he->data_.name.platform ),
                QString::fromStdString( he->data_.name.component ),
                QString::fromStdString( he->data_.provides[j].name ),
                isProvided,
                QString::fromStdString( he->data_.address ),
                QString::fromStdString( he->data_.provides[j].id ),
                he->data_.provides[j].isReachable,
                he->data_.timeUp );
        }
        // required interfaces
        for ( unsigned int j=0; j<he->data_.requires.size(); ++j ) {
            bool isProvided = false;
            model_.setInterface( 
                QString::fromStdString( he->data_.locatorString ),
                QString::fromStdString( he->data_.adminAddress ),
                QString::fromStdString( he->data_.name.platform ),
                QString::fromStdString( he->data_.name.component ),
                QString::fromStdString( orcaice::toString( he->data_.requires[j].name ) ),
                isProvided,
                QString::fromStdString( he->data_.address ),
                QString::fromStdString( he->data_.requires[j].id ),
                he->data_.requires[j].isReachable,
                he->data_.timeUp );
        }
    }

} 

} // namespace
