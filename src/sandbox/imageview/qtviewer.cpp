#include "qtviewer.h" 

using namespace imageview;

QtViewer::QtViewer( const orcaice::Context& context ) :
    Viewer(context)
{
}

QtViewer::~QtViewer()
{
}

void QtViewer::initialise()
{
    // start the Qt application in a separate thread
    qtViewerApp_ = new QtViewerApp( context_ );
    qtViewerApp_->start();
        // shutdown the component if the application window is closed
    // context_.communicator()->shutdown();

//     
//     // normally ctrl-c handler does this, now we have to because UserThread keeps the thread
//    context_.communicator()->shutdown();
}

void QtViewer::display( orca::ImageDataPtr image )
{
        // copy image into queue for the Qt Application
        // std::cout << "pushing image to app" << std::endl;
        qtViewerApp_->push(image);

}
