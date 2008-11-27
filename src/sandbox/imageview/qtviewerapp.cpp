#include "qtviewerapp.h"

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace imageview;

QtViewerApp::QtViewerApp( const orcaice::Context &context ) :
    imageQueue_(new ImageQueue(1)),
    context_(context)
{
}

QtViewerApp::~QtViewerApp()
{
}

void 
QtViewerApp::run()
{
    //create the event loop
    int argc = 0;
    char **argv = 0;
    
    QApplication app(argc, argv);
    QWidget* window = new QWidget();
    window->setWindowTitle("Orca Image Viewer");

    // create and show viewer and some information in labels
    ViewWidget* viewer = new ViewWidget(imageQueue_);

    QLabel* fpsLabel = new QLabel("FPS: ");
    QLabel* fpsDisplay = new QLabel;
    QHBoxLayout* hbox1 = new QHBoxLayout;
    hbox1->addWidget(fpsLabel);
    hbox1->addWidget(fpsDisplay);
    QWidget* labels = new QWidget;
    labels->setLayout(hbox1);
    labels->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(viewer);
    vbox->addWidget(labels);

    window->setLayout(vbox);
    window->show();

    // connect signal from timer to slot of widget to update
    QObject::connect(imageQueue_, SIGNAL(imagePushed()), viewer, SLOT(updateGL()));
    QObject::connect(viewer, SIGNAL(fpsChanged(double)), fpsDisplay, SLOT(setNum(double)));
    
    // start QApplication event loop
    app.exec();
    
    // context_.communicator()->shutdown();
    
}

void
QtViewerApp::push( orca::ImageDataPtr image )
{
    // std::cout<<"pushing image"<<std::endl;
    imageQueue_->push( image );
}
