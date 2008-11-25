#include "qtviewer.h" 

#include <orcaifaceutil/datetime.h>

using namespace imageview;

QtViewer::QtViewer()
{
}

QtViewer::~QtViewer()
{
    // delete imageQueue_;
    delete qtViewerApp_;
}

void QtViewer::initialise()
{
    // start the Qt application in a separate thread
    qtViewerApp_ = new QtViewerApp();
    qtViewerApp_->start();
//     
//     // normally ctrl-c handler does this, now we have to because UserThread keeps the thread
//     context().communicator()->shutdown();
}

void QtViewer::display( orca::ImageDataPtr image )
{
        // copy image into queue for the Qt Application\
        // std::cout << "pushing image to app" << std::endl;
        qtViewerApp_->push(image);

}
