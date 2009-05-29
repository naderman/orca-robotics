/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef VELOCITYCONTROL2DELEMENT_H
#define VELOCITYCONTROL2DELEMENT_H

#include <QObject>
#include <orcaice/context.h>
#include <hydroqguielementutil/guielement.h>

class QSplitter;

namespace hydroqguielementutil {
    class IHumanManager;
    class ShortcutKeyManager;
}

namespace orcateleop {
    class NetworkThread;
}

namespace orcaqgui2d {
    
    class VelocityControlDisplay;
    
// A simple teleoperation element
// Captures keypress events and sets up a display for user input
// The actual networking is handled by a orcateleop library class
// author: Tobias Kaupp    
class VelocityControl2dElement : public QObject,
                                 public hydroqguielementutil::GuiElement
{
    Q_OBJECT

    public: 
        VelocityControl2dElement(const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                                 const orcaice::Context                     &context,
                                 const std::string                          &proxyString,
                                 hydroqguielementutil::IHumanManager        &humanManager,
                                 hydroqguielementutil::ShortcutKeyManager   &shortcutKeyManager,
                                 QSplitter                                  *spaceBottomRight = 0 ); 
        ~VelocityControl2dElement();
        
        virtual bool isInGlobalCS() {return false;};

        
    private slots:
        void incrementSpeed();
        void decrementSpeed();
        void incrementTurnrate();
        void decrementTurnrate();
        void stopTurn();
        void stopMovementButTurn();
        void stop();
        
    private:
        void setupButtons();
        hydroqguielementutil::IHumanManager &humanManager_;
        hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager_;
        orcateleop::NetworkThread* networkThread_;
        VelocityControlDisplay* teleopDisplay_;
};

}

#endif
