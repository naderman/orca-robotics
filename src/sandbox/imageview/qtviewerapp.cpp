#include "qtviewerapp.h"

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace imageview;

QtViewerApp::QtViewerApp() :
    imageQueue_(new ImageQueue(1))
{
}

QtViewerApp::~QtViewerApp()
{
    delete viewer_;
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
    viewer_ = new ViewWidget(imageQueue_);

    QLabel * fpsLabel = new QLabel("FPS: ");
    QLabel * fpsDisplay = new QLabel;
    QHBoxLayout *hbox1 = new QHBoxLayout;
    hbox1->addWidget(fpsLabel);
    hbox1->addWidget(fpsDisplay);
    QWidget* labels = new QWidget;
    labels->setLayout(hbox1);
    labels->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(viewer_);
    vbox->addWidget(labels);

    window->setLayout(vbox);
    window->show();

    // connect signal from timer to slot of widget to update
    QObject::connect(imageQueue_, SIGNAL(imagePushed()), viewer_, SLOT(updateGL()));
    QObject::connect(viewer_, SIGNAL(fpsChanged(double)), fpsDisplay, SLOT(setNum(double)));
    
    // start QApplication event loop
    app.exec();
}

void
QtViewerApp::push( orca::ImageDataPtr image )
{
    // std::cout<<"pushing image"<<std::endl;
    imageQueue_->push( image );
}
