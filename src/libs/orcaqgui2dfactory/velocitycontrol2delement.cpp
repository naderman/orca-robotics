/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <QAction>
#include <QSplitter>
#include <hydrointerfaces/humaninput2d.h>
#include <orcaice/orcaice.h>
#include <orcateleop/networkthread.h>
#include "velocitycontroldisplay.h"
#include "velocitycontrol2delement.h"

using namespace std;

namespace orcaqgui2d {
    
VelocityControl2dElement::VelocityControl2dElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                                                    const orcaice::Context                     &context )
    : GuiElement(guiElementInfo)
{
    _stuff.humanManager->showDialogWarning("You are about to connect a Teleoperation element. Be careful...");
    
    // setup the buttons and connect to slots
    setupButtons();
    
    //
    // interface properties
    //
    Ice::PropertiesPtr prop = context.properties();
    prop->setProperty( context.tag()+".Requires.Generic.Proxy", guiElementInfo.uniqueId.toStdString() );
    
    std::string prefix = context.tag() + ".Config.";
    double repeatInterval = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"VelocityControl2d.RepeatInterval", 0.2 );
    stringstream ss; ss << repeatInterval;
    prop->setProperty( prefix+"RepeatInterval", ss.str() );
    
    // setup display
    teleopDisplay_ = new VelocityControlDisplay( guiElementInfo.platform.toStdString(), _stuff.spaceBottomRight );
    
    // setup network thread
    networkThread_ = new orcateleop::NetworkThread( (orcateleop::Display*)teleopDisplay_, context );
    networkThread_->start();
    
} 

VelocityControl2dElement::~VelocityControl2dElement()
{
    // send a stop command to robot
    stop();
    
    // clean up networkthread
    gbxiceutilacfr::stopAndJoin( networkThread_ );
    
    // delete display
    if (teleopDisplay_!=0) 
        delete teleopDisplay_;
}

void
VelocityControl2dElement::setupButtons()
{    
    // increment speed
    {
        QAction* incrementSpeed = new QAction(this);
        connect( incrementSpeed, SIGNAL(triggered()), this, SLOT(incrementSpeed()) ); 
        speedUpKey_.reset( new hydroqguielementutil::ShortcutKeyReservation( incrementSpeed,
                                                                             QKeySequence(Qt::Key_I),
                                                                             true,
                                                                             this,
                                                                             *_stuff.shortcutKeyManager ) );
    }   
    
    // decrement speed
    {
        QAction* decrementSpeed = new QAction(this);
        connect( decrementSpeed, SIGNAL(triggered()), this, SLOT(decrementSpeed()) ); 
        slowDownKey_.reset( new hydroqguielementutil::ShortcutKeyReservation( decrementSpeed,
                                                                              QKeySequence(Qt::Key_K),
                                                                              true,
                                                                              this,
                                                                              *_stuff.shortcutKeyManager ) );
    } 
    
    // increment turnrate
    {
        QAction* incrementTurnrate = new QAction(this);
        connect( incrementTurnrate, SIGNAL(triggered()), this, SLOT(incrementTurnrate()) );
        incrementTurnrateKey_.reset( new hydroqguielementutil::ShortcutKeyReservation( incrementTurnrate,
                                                                                       QKeySequence(Qt::Key_J),
                                                                                       true,
                                                                                       this,
                                                                                       *_stuff.shortcutKeyManager ) );
    } 
    
    // decrement turnrate
    {
        QAction* decrementTurnrate = new QAction(this);
        connect( decrementTurnrate, SIGNAL(triggered()), this, SLOT(decrementTurnrate()) ); 
        decrementTurnrateKey_.reset( new hydroqguielementutil::ShortcutKeyReservation( decrementTurnrate,
                                                                                       QKeySequence(Qt::Key_L),
                                                                                       true,
                                                                                       this,
                                                                                       *_stuff.shortcutKeyManager ) );
    }
    
    // stop turning
    {
        QAction* stopTurn = new QAction(this);
        connect( stopTurn, SIGNAL(triggered()), this, SLOT(stopTurn()) );
        stopRotationKey_.reset( new hydroqguielementutil::ShortcutKeyReservation( stopTurn,
                                                                                  QKeySequence(Qt::Key_U),
                                                                                  true,
                                                                                  this,
                                                                                  *_stuff.shortcutKeyManager ) );
    }    
    
    // stop moving but still turn
    {
        QAction* stopMovementButTurn = new QAction(this);
        connect( stopMovementButTurn, SIGNAL(triggered()), this, SLOT(stopMovementButTurn()) ); 
        stopTranslationKey_.reset( new hydroqguielementutil::ShortcutKeyReservation( stopMovementButTurn,
                                                                                     QKeySequence(Qt::Key_O),
                                                                                     true,
                                                                                     this,
                                                                                     *_stuff.shortcutKeyManager ) );
    }
        
    // stop
    {
        QAction* stop = new QAction(this);
        connect( stop, SIGNAL(triggered()), this, SLOT(stop()) ); 
        stopAllKey_.reset( new hydroqguielementutil::ShortcutKeyReservation( stop,
                                                                             QKeySequence(Qt::Key_Space),
                                                                             true,
                                                                             this,
                                                                             *_stuff.shortcutKeyManager ) );
    }
}

void
VelocityControl2dElement::incrementSpeed()
{
    hydrointerfaces::HumanInput2d::Command command;
    command.resetIncremental();
    command.longitudinal = 1.0;
    networkThread_->newMixedCommand( command );    
}

void 
VelocityControl2dElement::decrementSpeed()
{
    hydrointerfaces::HumanInput2d::Command command;
    command.resetIncremental();
    command.longitudinal = -1.0;
    networkThread_->newMixedCommand( command );    
}

void 
VelocityControl2dElement::incrementTurnrate()
{
    hydrointerfaces::HumanInput2d::Command command;
    command.resetIncremental();
    command.angular = 1.0;
    networkThread_->newMixedCommand( command );   
}

void 
VelocityControl2dElement::decrementTurnrate()
{
    hydrointerfaces::HumanInput2d::Command command;
    command.resetIncremental();
    command.angular = -1.0;
    networkThread_->newMixedCommand( command );   
}

void 
VelocityControl2dElement::stopTurn()
{
    hydrointerfaces::HumanInput2d::Command command;
    command.resetIncremental();
    command.isAngularIncrement = false;
    command.angular = 0.0;
    networkThread_->newMixedCommand( command );   
}

void 
VelocityControl2dElement::stopMovementButTurn()
{
    hydrointerfaces::HumanInput2d::Command command;
    command.resetIncremental();
    command.isLongIncrement = false;
    command.longitudinal = 0.0;
    networkThread_->newMixedCommand( command );   
}

void 
VelocityControl2dElement::stop()
{
    hydrointerfaces::HumanInput2d::Command command;
    command.resetAbsolute();
    networkThread_->newMixedCommand( command );   
}

}
