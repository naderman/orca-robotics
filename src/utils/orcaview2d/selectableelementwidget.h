#ifndef _SELECTABLEELEMENTWIDGET_H
#define _SELECTABLEELEMENTWIDGET_H

#include <QSplitter>
#include <hydroutil/jobqueue.h>
#include <orcaice/context.h>
#include <orcaqguielementmodelview/guielementmodel.h>
#include <orcaqguielementmodelview/guielementview.h>
#include <orcacm/orcacm.h>
#include <orcaqcm/orcaqcm.h>
#include "platformfocuscombo.h"

class QTreeView;
class QItemDelegate;
class QTimer;

//
// A Widget which allows users to select (with the mouse) items from a
// view of the Registry, and transfer them to the list of GuiElements.
//
class SelectableElementWidget : public QSplitter
{
    Q_OBJECT

public: 

    SelectableElementWidget( hydroqgui::PlatformFocusManager &platformFocusManager,
                             hydroutil::JobQueue             &jobQueue,
                             const orcaice::Context          &context,
                             QWidget                         *parent=NULL );

    void init( orcaqgemv::GuiElementModel *guiElemModel,
               QMainWindow                &mainWindow );

private slots:

    void updateRegistryView();
    void reloadRegistryView();

private: 

    // registry
    orcaqcm::OcmModel           *regModel_;
    QTreeView                   *regView_;
    QItemDelegate               *regDelegate_;

    // gui element list
    orcaqgemv::GuiElementModel             *elemModel_;
    orcaqgemv::GuiElementView              *elemView_;

    // display
    QWidget                     *displayView_;

    QTimer* regTimer_;
    QTimer* displayTimer_;
    std::vector<std::string>    supportedInterfaces_;

    // platform focus box
    PlatformFocusCombo *platformFocusCombo_;
    
    int displayRefreshTime_;

    hydroutil::JobQueue    &jobQueue_;
    orcaice::Context        context_;

};

#endif
