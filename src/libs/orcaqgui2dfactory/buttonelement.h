#ifndef ORCAQGUI2DFACTORY_BUTTONELEMENT_H
#define ORCAQGUI2DFACTORY_BUTTONELEMENT_H

#include <orcaqgui2d/guielement2d.h>
#include <orca/button.h>
#include <string>
#include <orcaice/context.h>
#include <hydroqgui/hydroqgui.h>

namespace orcaqgui2d {

//!
//! @author Alex Brooks
//!
class ButtonElement : public orcaqgui2d::GuiElement2d
{

public: 

    ButtonElement( const orcaice::Context   &context,
                   const std::string        &proxyString,
                   hydroqgui::IHumanManager &humanManager );

    virtual QStringList contextMenu();
    virtual void execute( int action );

    // This function doesn't need an implementation coz we don't paint anything.
    bool isInGlobalCS() { return false; }    
    // This function doesn't need an implementation coz we don't paint anything.
    bool paintThisLayer(int z) const { return false; }    

private: 

    void connect();
    void press();

    orcaice::Context context_;
    std::string      proxyString_;
    orca::ButtonPrx  buttonPrx_;

    hydroqgui::IHumanManager &humanManager_;

    bool isConnected_;
};

}

#endif
