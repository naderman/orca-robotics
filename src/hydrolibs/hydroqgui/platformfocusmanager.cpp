#include "platformfocusmanager.h"
#include <iostream>
#include "exceptions.h"

using namespace std;

namespace hydroqgui {

PlatformFocusChangeReceiver::PlatformFocusChangeReceiver( PlatformFocusManager &platformFocusManager )
    : platformFocusManager_(platformFocusManager)
{
    platformFocusManager_.registerForPlatformFocusChanges( this );
}

PlatformFocusChangeReceiver::~PlatformFocusChangeReceiver()
{
    platformFocusManager_.unRegisterForPlatformFocusChanges( this );
}

//////////////////////////////////////////////////////////////////////

PlatformFocusManager::PlatformFocusManager()
    : platformInFocus_("global")
{
}

void
PlatformFocusManager::setFocusPlatform( const QString &platformName, PlatformFocusChangeReceiver *receiver )
{
    IceUtil::Mutex::Lock lock(mutex_);

    platformInFocus_ = platformName;
    for ( size_t i=0; i < changeReceivers_.size(); i++ )
    {
        if ( changeReceivers_[i] != receiver )
            changeReceivers_[i]->platformFocusChanged( platformName );
    }
}

void 
PlatformFocusManager::registerForPlatformFocusChanges( PlatformFocusChangeReceiver *receiver )
{
    IceUtil::Mutex::Lock lock(mutex_);

    std::vector<PlatformFocusChangeReceiver*>::const_iterator it = std::find( changeReceivers_.begin(),
                                                                              changeReceivers_.end(),
                                                                              receiver );
    if ( it != changeReceivers_.end() )
    {
        throw Exception( ERROR_INFO, "receiver tried to register twice!!" );
    }
    changeReceivers_.push_back( receiver );
}

void
PlatformFocusManager::unRegisterForPlatformFocusChanges( PlatformFocusChangeReceiver *receiver )
{
    IceUtil::Mutex::Lock lock(mutex_);

    std::vector<PlatformFocusChangeReceiver*>::iterator it = std::find( changeReceivers_.begin(),
                                                                        changeReceivers_.end(),
                                                                        receiver );
    if ( it == changeReceivers_.end() )
    {
        cout << "ERROR(platformfocusmanager.cpp): unRegisterForPlatformFocusChanges(): receiver wasn't registered!" << endl;
    }
    else
    {
        changeReceivers_.erase( it );
    }        
}


}

