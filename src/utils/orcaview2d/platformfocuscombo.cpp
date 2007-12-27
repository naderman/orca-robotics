#include "platformfocuscombo.h"
#include <iostream>

using namespace std;

PlatformFocusCombo::PlatformFocusCombo( hydroqgui::PlatformFocusManager &platformFocusManager,
                                        QWidget *parent )
    : QComboBox(parent),
      PlatformFocusChangeReceiver(platformFocusManager),
      platformFocusManager_(platformFocusManager)
{
    setMinimumHeight(20);
    setMaximumHeight(20);
    setFont( QFont(font().family(), 12, QFont::Bold) );
    connect( this,
             SIGNAL(highlighted(const QString&)),
             this,
             SLOT(changePlatformFocus(const QString&)));
}

void 
PlatformFocusCombo::changePlatformFocus(const QString& platform)
{
    platformFocusManager_.setFocusPlatform( platform, this );
}

void 
PlatformFocusCombo::platformFocusChanged(const QString& newPlatformName)
{
    cout << "TRACE(platformfocuscombo.cpp): platformFocusChanged(): platform: " << newPlatformName.toStdString() << endl;
    
    int i = findText( newPlatformName );
    setCurrentIndex( i );
}

void 
PlatformFocusCombo::addPlatformToList(const QString& platform)
{
    assert(platform!="");

    cout<<"TRACE(platformfocuscombo.cpp): received newPlatform signal: " << platform.toStdString() << endl;
    
    // if it gets here, the platform is a new one
    addItem( platform );
}

void
PlatformFocusCombo::removePlatformFromList(const QString& platform)
{
    cout<<"TRACE(platformfocuscombo.cpp): received removePlatformFromList signal: " << platform.toStdString() << endl;
    int index = findText( platform );
    removeItem( index );    
}

