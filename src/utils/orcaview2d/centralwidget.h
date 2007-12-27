#ifndef _CENTRALWIDGET_H
#define _CENTRALWIDGET_H

#include <QSplitter>
#include <vector>
#include <hydroutil/jobqueue.h>
#include <orcaice/context.h>
#include <hydroqgui/ihumanmanager.h>
#include <orcaqguielementmodelview/guielementmodel.h>
#include <orcaqguielementmodelview/guielementview.h>
#include "platformfocuscombo.h"

namespace orcaqcm {
    class OcmModel;
};

class QTreeView;
class QItemDelegate;
class QSplitter;
class QTimer;

//
// The CentralWidget manages the layout of the GUI, and regular re-painting of the display view.
//
//
// Interaction with the JobQueue:
// - when you need to update the information about the content of the registry, create 
//   orcaqcm::GetComponentsJob and add it to the JobQueue. The job needs a pointer to an instance
//   of orcaqcm::OcmModel.
// - the rest will happen automatically in one of the threads of the JobQueue: the job will get
//   the data from the registry, transfer it to the OcmModel, which will update and repaint itself.
//
class CentralWidget : public QSplitter
{
    Q_OBJECT

public: 

    CentralWidget( QWidget &leftSideWidget,
                   int      displayRefreshTime );

    void init( orcaqgemv::GuiElementModel *guiElemModel,
               QWidget                    *displayView );

private slots:

    void updateDisplayView();

private: 

    // The widget on the left
    QWidget &leftSide_;

    // gui element list
    orcaqgemv::GuiElementModel             *elemModel_;

    // display
    QWidget                     *displayView_;

    QTimer* displayTimer_;
    std::vector<std::string>    supportedInterfaces_;

    int displayRefreshTime_;
};

#endif
