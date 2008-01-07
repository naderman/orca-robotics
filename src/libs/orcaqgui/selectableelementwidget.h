#ifndef ORCAQGUI_SELECTABLEELEMENTWIDGET_H
#define ORCAQGUI_SELECTABLEELEMENTWIDGET_H

#include <QSplitter>
#include <hydroiceutil/jobqueue.h>
#include <orcaice/context.h>
#include <orcaqguielementmodelview/guielementmodel.h>
#include <orcaqguielementmodelview/guielementview.h>
#include <orcacm/orcacm.h>
#include <orcaqcm/orcaqcm.h>
#include "platformfocuscombo.h"

class QTreeView;
class QItemDelegate;
class QTimer;

namespace orcaqgui {

//!
//! A Widget which allows users to select (with the mouse) items from a
//! view of the Registry, and transfer them to the list of GuiElements.
//!
//!
//! Interaction with the JobQueue:
//! - when you need to update the information about the content of the registry, create 
//!   orcaqcm::GetComponentsJob and add it to the JobQueue. The job needs a pointer to an instance
//!   of orcaqcm::OcmModel.
//! - the rest will happen automatically in one of the threads of the JobQueue: the job will get
//!   the data from the registry, transfer it to the OcmModel, which will update and repaint itself.
//!
class SelectableElementWidget : public QSplitter
{
    Q_OBJECT

public: 

    //! Negative regRefreshPeriodSec means never refresh automatically.
    SelectableElementWidget( hydroqgui::PlatformFocusManager &platformFocusManager,
                             hydroiceutil::JobQueue             &jobQueue,
                             const orcaice::Context          &context,
                             orcaqgemv::GuiElementModel      *guiElementModel,
                             QMainWindow                     &mainWindow,
                             int                              regRefreshPeriodSec=-1,
                             QWidget                         *parent=NULL );

private slots:

    void updateRegistryView();
    void reloadRegistryView();

private: 

    // registry
    orcaqcm::OcmModel           *regModel_;
    QTreeView                   *regView_;
    QItemDelegate               *regDelegate_;

    // gui element list
    orcaqgemv::GuiElementView              *elemView_;

    // display
    QWidget                     *displayView_;

    QTimer* regTimer_;
    QTimer* displayTimer_;
    std::vector<std::string>    supportedInterfaces_;

    // platform focus box
    PlatformFocusCombo *platformFocusCombo_;
    
    int displayRefreshTime_;

    hydroiceutil::JobQueue    &jobQueue_;
    orcaice::Context        context_;

};

}

#endif
