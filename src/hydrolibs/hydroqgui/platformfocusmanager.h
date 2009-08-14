#ifndef HYDROQGUI_PLATFORMFOCUSMANAGER_H
#define HYDROQGUI_PLATFORMFOCUSMANAGER_H

#include <QString>
#include <vector>
#include <hydroutil/uncopyable.h>
#include <IceUtil/IceUtil.h>

namespace hydroqgui {

class PlatformFocusManager;

//!
//! @brief Anyone interested in being notified about changes in platform focus
//!        should inherit from this.
//!
//! @author Alex Brooks
//!
class PlatformFocusChangeReceiver : public hydroutil::Uncopyable
{
public:

    PlatformFocusChangeReceiver( PlatformFocusManager &platformFocusManager );
    virtual ~PlatformFocusChangeReceiver();

    virtual void platformFocusChanged( const QString &newPlatformName )=0;

private:

    PlatformFocusManager &platformFocusManager_;
};

//!
//! @brief Manages which platform is in focus to help the user understand which platform he is interacting with.
//!        E.g. only the elements on the platform in focus are displayed and receive key presses and mouse events.
//!        The special platform "global" means everyone is in focus.
//!
//! @author Alex Brooks
//!
class PlatformFocusManager : public hydroutil::Uncopyable
{

public: 

    PlatformFocusManager();

    const QString &platformInFocus() const { return platformInFocus_; }

    //! The named receiver (eg. the caller of this function) won't get notified of the change.
    void setFocusPlatform( const QString &platformName, PlatformFocusChangeReceiver *receiver=NULL );

    void registerForPlatformFocusChanges( PlatformFocusChangeReceiver *receiver );
    void unRegisterForPlatformFocusChanges( PlatformFocusChangeReceiver *receiver );

private: 

    QString platformInFocus_;
    std::vector<PlatformFocusChangeReceiver*> changeReceivers_;

    IceUtil::Mutex mutex_;
};

}

#endif
