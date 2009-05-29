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
#include <hydroqguielementutil/ihumanmanager.h>
#include <hydroqguielementutil/shortcutkeymanager.h>
#include <orcaice/orcaice.h>
#include <orcateleop/networkthread.h>
#include "velocitycontroldisplay.h"
#include "velocitycontrol2delement.h"

using namespace std;

namespace orcaqgui2d {
    
VelocityControl2dElement::VelocityControl2dElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                                                    const orcaice::Context                     &context,
                                                    const std::string                          &proxyString,
                                                    hydroqguielementutil::IHumanManager        &humanManager,
                                                    hydroqguielementutil::ShortcutKeyManager   &shortcutKeyManager,
                                                    QSplitter                                  *spaceBottomRight )
    : GuiElement(guiElementInfo),
      humanManager_(humanManager),
      shortcutKeyManager_(shortcutKeyManager)
{
    humanManager.showDialogWarning("You are about to connect a Teleoperation element. Be careful...");
    
    // setup the buttons and connect to slots
    setupButtons();
    
    //
    // interface properties
    //
    Ice::PropertiesPtr prop = context.properties();
    prop->setProperty( context.tag()+".Requires.Generic.Proxy", proxyString );
    
    std::string prefix = context.tag() + ".Config.";
    double repeatInterval = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"VelocityControl2d.RepeatInterval", 0.2 );
    stringstream ss; ss << repeatInterval;
    prop->setProperty( prefix+"RepeatInterval", ss.str() );
    
    // setup display
    teleopDisplay_ = new VelocityControlDisplay( guiElementInfo.platform.toStdString(), spaceBottomRight );
    
    // setup network thread
    networkThread_ = new orcateleop::NetworkThread( (orcateleop::Display*)teleopDisplay_, context );
    networkThread_->start();
    
} 

VelocityControl2dElement::~VelocityControl2dElement()
{
    // send a stop command to robot
    stop();
    
    //
    // clean up key subscriptions
    //
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_I), this );
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_J), this );
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_K), this );
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_L), this );
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_U), this );
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_O), this );
    shortcutKeyManager_.unsubscribeFromShortcutKey( QKeySequence(Qt::Key_Space), this );
    
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
        shortcutKeyManager_.subscribeToShortcutKey( incrementSpeed, QKeySequence(Qt::Key_I), true, this );
    }   
    
    // decrement speed
    {
        QAction* decrementSpeed = new QAction(this);
        connect( decrementSpeed, SIGNAL(triggered()), this, SLOT(decrementSpeed()) ); 
        shortcutKeyManager_.subscribeToShortcutKey( decrementSpeed, QKeySequence(Qt::Key_K), true, this );
    } 
    
    // increment turnrate
    {
        QAction* incrementTurnrate = new QAction(this);
        connect( incrementTurnrate, SIGNAL(triggered()), this, SLOT(incrementTurnrate()) ); 
        shortcutKeyManager_.subscribeToShortcutKey( incrementTurnrate, QKeySequence(Qt::Key_J), true, this );
    } 
    
    // increment turnrate
    {
        QAction* decrementTurnrate = new QAction(this);
        connect( decrementTurnrate, SIGNAL(triggered()), this, SLOT(decrementTurnrate()) ); 
        shortcutKeyManager_.subscribeToShortcutKey( decrementTurnrate, QKeySequence(Qt::Key_L), true, this );
    }
    
    // stop turning
    {
        QAction* stopTurn = new QAction(this);
        connect( stopTurn, SIGNAL(triggered()), this, SLOT(stopTurn()) ); 
        shortcutKeyManager_.subscribeToShortcutKey( stopTurn, QKeySequence(Qt::Key_U), true, this );
    }    
    
    // stop moving but still turn
    {
        QAction* stopMovementButTurn = new QAction(this);
        connect( stopMovementButTurn, SIGNAL(triggered()), this, SLOT(stopMovementButTurn()) ); 
        shortcutKeyManager_.subscribeToShortcutKey( stopMovementButTurn, QKeySequence(Qt::Key_O), true, this );
    }
        
    // stop
    {
        QAction* stop = new QAction(this);
        connect( stop, SIGNAL(triggered()), this, SLOT(stop()) ); 
        shortcutKeyManager_.subscribeToShortcutKey( stop, QKeySequence(Qt::Key_Space), true, this );
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

